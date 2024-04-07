//
// Created by patte on 12/24/2020.
//

#include "ControllerGUI.h"
#include "UIGroup.h"
#include "FontManager.h"
#include "Label.h"
#include "Dialog.h"
#include "json.hpp"

const int SCREEN_WIDTH = 480;
const int SCREEN_HEIGHT = 800;

using namespace nlohmann;   //json

ControllerGUI::ControllerGUI(bool fullscreen,const char* host,unsigned short port,const char* engine,const char* pgnPathname)
    : Window("controllergui",0,0,SCREEN_WIDTH,SCREEN_HEIGHT),
      m_window(nullptr),
      m_renderer(nullptr),
      m_running(false),
      m_host(host),
      m_port(port),
      m_board(nullptr),
      m_movesPanel(nullptr),
      m_connector(nullptr),
      m_uci(engine),
      m_pgnFile(pgnPathname)
{
    printf("using chess engine [%s] host=[%s]\n",engine,m_host.c_str());
    m_board=new Board(0,0,SCREEN_WIDTH,SCREEN_WIDTH);
    m_movesPanel=new MovesPanel("moves",SCREEN_WIDTH-200,480,200,320,m_board->rules());
    m_connector=new Connector();
    m_fullscreen=fullscreen;
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
        return;
    }
    m_window = SDL_CreateWindow(
            "Chessbox",
//            SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,
            50,50,
            SCREEN_WIDTH, SCREEN_HEIGHT,
            fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP:SDL_WINDOW_RESIZABLE);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    FontManager::instance()->add("small","Inconsolata-Medium.ttf",10);
    FontManager::instance()->add("normal","Inconsolata-Medium.ttf",16);
    FontManager::instance()->add("large","Inconsolata-Medium.ttf",26);
    m_renderer = SDL_CreateRenderer(m_window, -1, 0);
    if(!m_renderer) {
        printf("m_renderer error %s\n",SDL_GetError());
    }
}

ControllerGUI::~ControllerGUI() {
    for (list<Component *>::iterator it = m_components.begin(); it != m_components.end(); it++) {
        delete *it;
    }
    if(m_connector)
        delete m_connector;
    m_connector=nullptr;
}

void ControllerGUI::initComponents() {
    Sprite* logo=new Sprite("logo");
    logo->load(m_renderer, "assets/logo-sm.png", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
    addComponent(logo);

    m_movesPanel->init();

    int x=0,y=480,w=60,h=60,gap=5;
    snprintf(m_buffer,sizeof m_buffer,"L:%d",m_skillLevel);
    TextButton* level=new TextButton("levelbutton",m_buffer,x,y,w,h);

    x+=w+gap;
    snprintf(m_buffer,sizeof m_buffer,"S:%d",m_skillLevel);
    TextButton* skill=new TextButton("skillbutton",m_buffer,x,y,w,h);

    x+=w+gap;
    snprintf(m_buffer,sizeof m_buffer,"D:%d",m_depth);
    TextButton* depth=new TextButton("depthbutton",m_buffer,x,y,w,h);

    x+=w+gap;
    snprintf(m_buffer,sizeof m_buffer,"T:%d",m_movetime);
    TextButton* time=new TextButton("timebutton",m_buffer,x,y,w,h);

    x=0;
    y+=h+gap;
    Button* connectButton=new TextButton("connectbutton", "Connect", x, y, w, h);
    x+=w+gap;
    Button* loadButton=new TextButton("loadbutton", "Load", x, y, w, h);
    x+=w+gap;
    Button* exportButton=new TextButton("exportbutton", "Export", x, y, w, h);
    x+=w+gap;
    Button* dbButton=new TextButton("dbbutton", "DB", x, y, w, h);

    x=0;
    y+=h+gap;
    Button* hintButton=new TextButton("hintbutton", "Hint", x, y, w, h);
    x+=w+gap;
    Button* inspectButton=new TextButton("inspectbutton", "Inspect", x, y, w, h);
    x+=w+gap;
    Button* markButton=new TextButton("markbutton", "Mark", x, y, w, h);
    x+=w+gap;
    Button* flipButton=new TextButton("flipbutton", "Flip", x, y, w, h);

    x=0;
    y+=h+gap;
    Button* newGameWhite=new AnimButton("newgamewhite",m_renderer,"assets/button-player-white.png",1,x,y);
    x+=w+gap;
    Button* newGameBlack=new AnimButton("newgameblack",m_renderer,"assets/button-player-black.png",1,x,y);
    x+=w+gap;
    Button* newGameWhiteVsBlack=new AnimButton("newgamewhitevsblack",m_renderer,"assets/button-twoplayer-whiteblack.png",1,x,y);
    x+=w+gap;
    Button* newGameBlackVsWhite=new AnimButton("newgameblackvswhite",m_renderer,"assets/button-twoplayer-blackwhite.png",1,x,y);

    x=0;
    y+=h+gap;
    Button* menuButton=new TextButton("menubutton", "Menu", x, y, w, h);
    x+=w+gap;
    Button* fastBack=new AnimButton("fastbackbutton",m_renderer,"assets/button-fastback.png",1,x,y);
    x+=w+gap;
    Button* back=new AnimButton("backbutton",m_renderer,"assets/button-back.png",1,x,y);
    x+=w+gap;
    Button* fwd=new AnimButton("fwdbutton",m_renderer,"assets/button-fwd.png",1,x,y);

    addButton(level);
    addButton(skill);
    addButton(time);
    addButton(depth);

    addButton(connectButton);
    addButton(loadButton);
    addButton(exportButton);
    addButton(dbButton);

    addButton(hintButton);
    addButton(inspectButton);
    addButton(markButton);
    addButton(flipButton);

    addButton(newGameWhite);
    addButton(newGameBlack);
    addButton(newGameWhiteVsBlack);
    addButton(newGameBlackVsWhite);

    addButton(menuButton);
    addButton(fastBack);
    addButton(back);
    addButton(fwd);

    addComponent(level);
    addComponent(skill);
    addComponent(time);
    addComponent(depth);

    addComponent(connectButton);
    addComponent(loadButton);
    addComponent(exportButton);
    addComponent(dbButton);

    addComponent(hintButton);
    addComponent(inspectButton);
    addComponent(markButton);
    addComponent(flipButton);

    addComponent(newGameWhite);
    addComponent(newGameBlack);
    addComponent(newGameWhiteVsBlack);
    addComponent(newGameBlackVsWhite);

    addComponent(menuButton);
    addComponent(fastBack);
    addComponent(back);
    addComponent(fwd);

    addComponent(m_board);
    addComponent(m_movesPanel);
    m_board->loadPieces(m_renderer,"merida_new");
}

void ControllerGUI::startGame() {
    initComponents();
    m_uci.setDebug(true);
    m_uci.start();
    m_uci.discoverOptions();
    m_uci.setSpinOption(ENGINE_OPTION_SKILL_LEVEL,m_skillLevel);
#ifdef WIN32
#endif
    m_uci.newGame();
    show(m_renderer);   //main loop, returns when user exits
    m_uci.stop();
    SDL_DestroyWindow(m_window);
    SDL_DestroyRenderer(m_renderer);
    SDL_Quit();
}

void ControllerGUI::update(long ticks) {
    Window::update(ticks);
    if(m_connector->isConnected()) {
        try {
            char buffer[1024];
            if(m_connector->readline(buffer, sizeof(buffer))) {
                printf("Read from socket: %s\n", buffer);
                if(buffer[0]=='{') {
                    processJson(buffer);
                }
            }
        } catch(SocketInstanceException& e) {
            Dialog dlg("Error","Error communicating with controller",DIALOG_TYPE_OK);
            dlg.show(m_renderer);
            m_connector->close();
            Button* b=findButton("connectbutton");
            if(b) b->setChecked(false);
        }
    }
}

void ControllerGUI::processButtonClicked(Button *c) {
    if(!strcmp(c->id(),"connectbutton")) {
        if(m_connector && m_connector->isConnected()) {
            disconnectController();
        } else {
            connectController();
        }
    } else if(!strcmp(c->id(),"loadbutton")) {
        notImplemented();
//        loadGame("/t/sample.pgn"); //todo placeholder
    } else if(!strcmp(c->id(),"exportbutton")) {
        notImplemented();
    } else if(!strcmp(c->id(),"dbbutton")) {
        notImplemented();
    } else if(!strcmp(c->id(),"levelbutton")) {
        notImplemented();
    } else if(!strcmp(c->id(),"skillbutton")) {
        EngineSpinOption *op=static_cast<EngineSpinOption *>(m_uci.option(ENGINE_OPTION_SKILL_LEVEL));
        int skill=op->m_currentValue + 1;
        if(skill>op->maxValue())
            skill=op->minValue();
        m_uci.setSpinOption(ENGINE_OPTION_SKILL_LEVEL, skill);
        TextButton *t=static_cast<TextButton *>(c);
        snprintf(m_buffer, sizeof m_buffer, "S:%d", skill);
        t->setText(m_buffer);
    } else if(!strcmp(c->id(),"depthbutton")) {
        if(++m_depth>22)
            m_depth=0;
        m_uci.setDepth(m_depth);
        TextButton *t=static_cast<TextButton *>(c);
        snprintf(m_buffer, sizeof m_buffer, "D:%d", m_depth);
        t->setText(m_buffer);
    } else if(!strcmp(c->id(),"timebutton")) {
        if(m_movetime<10)
            m_movetime+=1;
        else if(m_movetime<100)
            m_movetime+=10;
        else
            m_movetime+=100;

        if(m_movetime>1000)
            m_movetime=0;
        m_uci.setMovetime(m_movetime);
        TextButton *t=static_cast<TextButton *>(c);
        snprintf(m_buffer, sizeof m_buffer, "T:%d", m_movetime);
        t->setText(m_buffer);
    } else if(!strcmp(c->id(),"hintbutton")) {
        notImplemented();
    } else if(!strcmp(c->id(),"inspectbutton")) {
        if(!m_connector->isConnected()) {
            notConnectedMessage();
        } else {
            if(c->isChecked()) {
                m_connector->inspect(false);
                c->setChecked(false);
            } else {
                m_connector->inspect(true);
                c->setChecked(true);
            }
        }
    } else if(!strcmp(c->id(),"markbutton")) {
        notImplemented();
    } else if(!strcmp(c->id(),"flipbutton")) {
        printf("flipped=%d\n",m_board->isFlipped());
        m_board->flip(!m_board->isFlipped());
        json j;
        j["action"]="flip";
        j["flipped"]=m_board->isFlipped();
        printf("cleared json=%s\n",j.dump().c_str());
        m_connector->send(j.dump().c_str());
    } else if(!strcmp(c->id(),"newgamewhite")) {
        //single player, user is white
        setupNewGame();
    } else if(!strcmp(c->id(),"newgameblack")) {
        //single player, user is black (board is flipped)
        notImplemented();
    } else if(!strcmp(c->id(),"newgamewhitevsblack")) {
        //2 player game, white is buttom, black is top
        notImplemented();
    } else if(!strcmp(c->id(),"newgameblackvswhite")) {
        //2 player game, black is bottom, white is top, (board is flipped)
        notImplemented();
    } else if(!strcmp(c->id(),"menubutton")) {
        const int numMoves =  44;
        const char* moves[numMoves] = {
                "d4",
                "d5",
                "c4",
                "c6",
                "e3",
                "Bf5",
                "Nc3",
                "e6",
                "Nf3",
                "Nd7",
                "a3",
                "Bd6",
                "c5",
                "Bc7",
                "b4",
                "e5",
                "Be2",
                "Ngf6",
                "Bb2",
                "e4",
                "Nd2",
                "h5",
                "h3",
                "Nf8",
                "a4",
                "Ng6",
                "b5",
                "Nh4",
                "g3",
                "Ng2+",
                "Kf1",
                "Nxe3+",
                "fxe3",
                "Bxg3",
                "Kg2",
                "Bc7",
                "Qg1",
                "Rh6",
                "Kf1",
                "Rg6",
                "Qf2",
                "Qd7",
                "bxc6",
                "bxc6"
        };

        for(int i=0; i<43; i++) {
            Move m;
            m.NaturalIn(m_board->rules(), moves[i]);
            m_movesPanel->add(m);
        }
    } else if(!strcmp(c->id(),"fastbackbutton")) {
        notImplemented();
    } else if(!strcmp(c->id(),"backbutton")) {
        notImplemented();
    } else if(!strcmp(c->id(),"fwdbutton")) {
        notImplemented();
    }
}

void ControllerGUI::connectController() {
    Dialog confirm("Confirm","Connect to Chessbox controller?",DIALOG_TYPE_YES_NO);
    int selection=confirm.show(m_renderer);
    if(DIALOG_SELECTED_YES==selection) {
        if(!m_connector->isConnected()) {
            try {
                printf("connecting to %s:%d\n",m_host.c_str(),m_port);
                m_connector->connect(m_host.c_str(), m_port);
                m_connector->waitline(m_buffer,sizeof(m_buffer));
                printf("read %s\n",m_buffer);
                Button* b=findButton("connectbutton");
                b->setChecked(true);
                m_connector->send("{\"action\":\"fen\"}");  //get initial board position
            } catch(GeneralException e) {
                Dialog dlg("Error", "Unable to connect to chessbox controller", DIALOG_TYPE_OK);
                dlg.show(m_renderer);
            }
        } else {
            Dialog msg("Message","Already connected",DIALOG_TYPE_OK);
            msg.show(m_renderer);
        }
    }
}

void ControllerGUI::disconnectController() {
    if(m_connector->isConnected()) {
        Dialog confirm("Confirm","Disconnect from Chessbox controller?",DIALOG_TYPE_YES_NO);
        int selection=confirm.show(m_renderer);
        if(DIALOG_SELECTED_YES==selection) {
            m_connector->close();
            Button *b=findButton("connectbutton");
            b->setChecked(false);
        }
    }
}

/** Show message that you must be connected. */
void ControllerGUI::notConnectedMessage() {
    Dialog msg("Chessbox","Must be connected to the board controller",DIALOG_TYPE_OK);
    msg.show(m_renderer);
}

/** Show message that you must be connected. */
void ControllerGUI::notImplemented() {
    Dialog msg("Chessbox","Not yet implemented",DIALOG_TYPE_OK);
    msg.show(m_renderer);
}

void ControllerGUI::setupNewGame() {
    if(!m_connector->isConnected()) {
        notConnectedMessage();
    } else {
        const char *fen="rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
        m_board->Forsyth(fen);
        m_uci.newGame();
        printf("LBP sending set position\n");
        json j;
        j["action"]="setposition";
        j["fen"]=fen;
        m_connector->send(j.dump().c_str());
        j.clear();
        j["action"]="flip";
        j["flipped"]=m_board->isFlipped();
        printf("cleared json=%s\n",j.dump().c_str());
        m_connector->send(j.dump().c_str());
        m_board->clearHighlights();
    }
}

void ControllerGUI::loadGame(const char* pathname) {
    FILE* fp=fopen(pathname,"r");
    if(fp) {

        fclose(fp);
    }
}
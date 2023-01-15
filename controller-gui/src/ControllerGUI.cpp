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
    printf("using chess engine [%s]\n",engine);
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
            300,100,
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

    int x=0,y=510,w=110,h=40,gap=10;
    Label* label=new Label("playersettingslabel",0,510,SCREEN_WIDTH/2,290);
    y+=gap+gap;
    label->setText("One Player Settings");
    snprintf(m_buffer,sizeof m_buffer,"Skill: %d",m_skillLevel);
    TextButton* skill=new TextButton("skillbutton",m_buffer,x,y,w,h);
    x+=w+gap;
    x=0;
    y+=h+gap;
    snprintf(m_buffer,sizeof m_buffer,"Depth: %d",m_depth);
    TextButton* depth=new TextButton("depthbutton",m_buffer,x,y,w,h);
    x+=w+gap;
    snprintf(m_buffer,sizeof m_buffer,"Time: %d",m_movetime);
    TextButton* time=new TextButton("timebutton",m_buffer,x,y,w,h);
    x+=w+gap;
    TextButton* level=new TextButton("levelbutton","Level: 7",x,y,w,h);

    x=0;
    y=800-130;
    w=h=60;
//    Button* settings=new AnimButton("settingsbutton",m_renderer,"assets/button-gear.png",1,x,y);
    Button* settings=new TextButton("settingsbutton","Connect",x,y,w,h);
    x+=w+gap;
    Button* hint=new TextButton("hintbutton","Hint",x,y,w,h);
    x+=w+gap;
    Button* inspect=new TextButton("inspectbutton","Inspect",x,y,w,h);
    x+=w+gap;
    Button* newGame=new TextButton("newgamebutton","New",x,y,w,h);

    x=0;
    y+=h+gap;
    Button* fastBack=new AnimButton("fastbackbutton",m_renderer,"assets/button-fastback.png",1,x,y);
    x+=w+gap;
    Button* back=new AnimButton("backbutton",m_renderer,"assets/button-back.png",1,x,y);
    x+=w+gap;
    Button* fwd=new AnimButton("fwdbutton",m_renderer,"assets/button-fwd.png",1,x,y);
    x+=w+gap;
    Button* fastFwd=new AnimButton("fastfwdbutton",m_renderer,"assets/button-fastfwd.png",1,x,y);
    x+=w+gap;

//    addButton(level);
    addButton(skill);
    addButton(time);
    addButton(depth);
    addButton(settings);
    addButton(hint);
    addButton(inspect);
    addButton(newGame);
    addButton(fastBack);
    addButton(back);
    addButton(fwd);
    addButton(fastFwd);

    addComponent(label);
//    addComponent(level);
    addComponent(skill);
    addComponent(time);
    addComponent(depth);
    addComponent(settings);
    addComponent(hint);
    addComponent(inspect);
    addComponent(newGame);
    addComponent(fastBack);
    addComponent(back);
    addComponent(fwd);
    addComponent(fastFwd);
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
            Button* b=findButton("settingsbutton");
            if(b) b->setChecked(false);
        }
    }
}

void ControllerGUI::processButtonClicked(Button *c) {
    if(!strcmp(c->id(),"settingsbutton")) {
        if(m_connector && m_connector->isConnected()) {
            disconnectController();
        } else {
            connectController();
        }
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
    } else if(!strcmp(c->id(),"newgamebutton")) {
        setupNewGame();
    } else if(!strcmp(c->id(),"hintbutton")) {
        notImplemented();
    } else if(!strcmp(c->id(),"depthbutton")) {
        if(++m_depth>22)
            m_depth=0;
        m_uci.setDepth(m_depth);
        TextButton *t=static_cast<TextButton *>(c);
        snprintf(m_buffer, sizeof m_buffer, "Depth: %d", m_depth);
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
        snprintf(m_buffer, sizeof m_buffer, "Time: %d", m_movetime);
        t->setText(m_buffer);
    } else if(!strcmp(c->id(),"skillbutton")) {
        EngineSpinOption *op=static_cast<EngineSpinOption *>(m_uci.option(ENGINE_OPTION_SKILL_LEVEL));
        int skill=op->m_currentValue + 1;
        if(skill>op->maxValue())
            skill=op->minValue();
        m_uci.setSpinOption(ENGINE_OPTION_SKILL_LEVEL, skill);
        TextButton *t=static_cast<TextButton *>(c);
        snprintf(m_buffer, sizeof m_buffer, "Skill: %d", skill);
        t->setText(m_buffer);
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
                Button* b=findButton("settingsbutton");
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
            Button *b=findButton("settingsbutton");
            b->setChecked(false);
        }
    }
}

/** Show message that you must be connected. */
void ControllerGUI::notConnectedMessage() {
    Dialog msg("Chessbox","You must be connected to the controller",DIALOG_TYPE_OK);
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
        json j;
        j["action"]="setposition";
        j["fen"]=fen;
        m_connector->send(j.dump().c_str());
        m_board->clearHighlights();
    }
}
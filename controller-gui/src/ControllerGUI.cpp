//
// Created by patte on 12/24/2020.
//

#include "ControllerGUI.h"
#include "UIGroup.h"
#include "FontManager.h"
#include "Label.h"

const int SCREEN_WIDTH = 480;
const int SCREEN_HEIGHT = 800;

ControllerGUI::ControllerGUI(bool fullscreen,const char* host,unsigned short port)
    : Window("controllergui",0,0,SCREEN_WIDTH,SCREEN_HEIGHT),
      m_window(nullptr),
      m_renderer(nullptr),
      m_running(false),
      m_host(host),
      m_port(port),
      m_board(nullptr),
      m_rules(),
      m_movesPanel(nullptr)
{
    m_board=new Board(0,0,SCREEN_WIDTH,SCREEN_WIDTH);
    m_movesPanel=new MovesPanel("moves",SCREEN_WIDTH-200,480,200,320,&m_rules);
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
    TextButton* level=new TextButton("levelbutton","Level: 7",x,y,w,h);
    x+=w+gap;
    TextButton* skill=new TextButton("skillbutton","Skill: 20",x,y,w,h);
    x=0;
    y+=h+gap;
    TextButton* time=new TextButton("timebutton","Time: 1000",x,y,w,h);
    x+=w+gap;
    TextButton* depth=new TextButton("depthbutton","Depth: 22",x,y,w,h);

    x=0;
    y=800-130;
    w=h=60;
    AnimButton* settings=new AnimButton("settingsbutton",m_renderer,"assets/button-gear.png",1,x,y);
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

    addButton(level);
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
    addComponent(level);
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
//    Sprite logo("logo");
//    logo.load(m_renderer, "assets/logo-sm.png", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
//    addComponent(&logo);

    show(m_renderer);
    SDL_DestroyWindow(m_window);
    SDL_DestroyRenderer(m_renderer);
    SDL_Quit();
}

void ControllerGUI::processButtonClicked(Button *c) {
    printf("%s clicked\n",c->id());
}
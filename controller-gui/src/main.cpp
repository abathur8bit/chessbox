/* *****************************************************************************
 * Created by Lee Patterson 12/19/2020
 *
 * Copyright 2019 Lee Patterson <https://github.com/abathur8bit>
 *
 * You may use and modify at will. Please credit me in the source.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * ******************************************************************************/

#include <stdio.h>
#include <string.h>

#include <algorithm>
#include <list>

#include <SDL.h>
#include <time.h>
#include <SDL_image.h>

#include "Sprite.h"
#include "Button.h"
#include "Board.h"
#include "UIGroup.h"
#include "thc.h"
#include "Label.h"
#include "MovesPanel.h"
#include "Connector.h"
#include "chessaction.hpp"
#include "json.hpp"
#include "ControllerGUI.h"
#include "Dialog.h"
#include "TextField.h"
#include "FontManager.h"

using namespace std;
using namespace nlohmann;   //trying this

char buffer[1024];
const int SCREEN_WIDTH = 480;
const int SCREEN_HEIGHT = 800;
int counter=0;
//#define NUM_MOVES 16

SDL_Window* window=NULL;
SDL_Renderer* renderer=NULL;
bool invalidated = true;
list<Component*> uistuff;
UIGroup buttonGroup("buttons",0,670,280,130);
//UIGroup movesGroup("moves",480-200,480,200,800-320);

char host[80]="192.168.1.54";
unsigned short port=9999;
Connector connector;
bool attemptConnect=true;
bool running=false;
Board board(0,0,480,480);
MovesPanel* movesPanel;
Sprite logo("logo");
Sprite movesBG("moves-bg1");
Label* blackClockText;
Label* whiteClockText;
int gameMovesIndex=0;
const char* gameMoves[] = {
        "d4","d5",
        "c4","c6",
        "Nf3","Nf6",
        "Nc3","e6",
        "Bg5","h6",
        "Bxf6","Qxf6",
        "e3","Nd7",
        "Rc1","g6",
        "Be2","Bg7",
        "cxd5","exd5",
        "b4","a6",
        "a4","O-O",
        "b5","axb5",
        "axb5","Qd6",
        "O-O","Nb6",
        "Qb3","Rb8",
        "Nd1","Bf5",
        "Nb2","Rfc8",
        "Nd3","Bxd3",
        "Qxd3","c5",
        "dxc5","Rxc5",
        "h4","Na4",
        "h5","Rbc8",
        "Rxc5","Nxc5",
        "Qc2","gxh5",
        "Nd4","Qg6",
        "Nf5","Bf8",
        "Rd1","Qe6",
        "Rc1","Nb3",
        "Qxc8","Nxc1",
        "Qxc1","Qxf5",
        "Qc7","Qb1+",
        "Bf1","d4",
        "exd4","Qd1",
        "Qe5","Bg7",
        "Qe8+","Bf8",
        "Qd8","Kg7",
        "Qd5","b6",
        "Qe5+","Kg8",
        "Qf6","Bg7",
        "Qxb6","Bxd4",
        "Qxh6","Qg4",
        "Qd6","Qd1",
        "Qd8+","Kh7",
        "Qc7","Kg7",
        "b6","Qg4",
        "b7","Qh4",
        "g3","Qf6",
        "Qc2","Qe5",
        "Qd3","Ba7",
        "Qf3","Qf6",
        "Qe2","Qc3",
        "Kh2","Qd4",
        "Qf3","Bb8",
        "Kh3","Bc7",
        "Be2","Bb8",
        "Bd1","f5",
        "Be2","f4",
        "Qxh5","Qxf2",
        "Qg5+","Kf7"
};

ChessAction* parseJson(const char* s) {
    json j = json::parse(s);
    ChessAction* c = new ChessAction(j);
    return c;
}

void validate() {
    invalidated=false;
}

void invalidate() {
    invalidated=true;
}

bool isInvalidated() {
    return invalidated;
}

void processMouseEvent(SDL_Event* event) {
    Component* result = buttonGroup.mouseEvent(event);
    if(result) {
//        printf("Event for %s\n",result->id());
        if(event->type == SDL_MOUSEBUTTONUP) {
            if (!strcmp(result->id(), "power")) {
                printf("Connecting to controller at %s:%d\n", host, port);
                connector.connect(host, port);
                printf("Connected to controller\n");
                Button *b = static_cast<Button *>(result);
                b->setChecked(true);
            } else if (!strcmp(result->id(), "settings")) {
                Dialog message("Demo","Fast bunnies",DIALOG_TYPE_OK_CANCEL);
                if(message.show(renderer)==DIALOG_SELECTED_OKAY) {
                    printf("user selected okay\n");
                } else {
                    printf("user selected cancel\n");
                }
            } else if (!strcmp(result->id(), "inspect")) {

            } else if (!strcmp(result->id(), "fwd")) {
                thc::Move mv;
                mv.NaturalIn(board.rules(), gameMoves[gameMovesIndex++]);
                movesPanel->add(mv);
            } else if (!strcmp(result->id(), "back")) {
                const char *fen = "rnbqkb1r/ppp1pppp/5n2/3p4/3P1B2/4P3/PPP2PPP/RN1QKBNR b KQkq - 0 3";
                board.Forsyth(fen);
            } else if (!strcmp(result->id(), "hint")) {
                snprintf(buffer,sizeof(buffer),"hint %d",++counter);
                movesPanel->add(buffer);
                int i = 0;
                Button *b = static_cast<Button *>(buttonGroup.find("hint"));
                if (b) {
                    b->setChecked(!b->isChecked());
                    board.highlightSquare(i, !board.isHighlighted(i));
                    i++;
                    board.highlightSquare(i, !board.isHighlighted(i));
                }
            }
        }
        invalidate();
    }

    //was processing the components here, then using a lamba. Will see about using a lambda again later.
//    for(list<Component*>::iterator it=buttonGroup.begin(); it != buttonGroup.end(); ++it) {
//        Button* b = static_cast<Button*>(*it);
//        b->mouseEvent(event,[](Button* b){printf("button %s was clicked size=%dx%d\n",b->id(),b->rect()->w,b->rect()->h);});  //called method signature is //bool Button::mouseEvent(SDL_Event* event,void(*f)(Button* b));
//    }
}

void coolSpot(bool fullscreen) {
    SDL_Rect r;
    SDL_Rect r2;
    SDL_Point center;
    Uint32 timer=0;

//    SDL_EnableScreenSaver();

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
        return;
    }

//    SDL_RendererInfo info;
//    int numRenderDrivers = SDL_GetNumRenderDrivers();
//    printf("Number of render driver: %d\n", numRenderDrivers);
//    for(int i=0; i < numRenderDrivers; i++)
//    {
//        if ( SDL_GetRenderDriverInfo(i,&info) == 0 )
//        {
//            printf("%s\n",info.name);
//        }
//    }

//    const char* drivername="RPI";
//    if (SDL_VideoInit(drivername) != 0) {
//        printf("unable to init video %s\n",SDL_GetError());
//    }

//    int err=SDL_CreateWindowAndRenderer(480,800,FULL_SCREEN_MODE,&m_window,&m_renderer);
//    if(err!=0) printf("SDL_CreateWindowAndRenderer error %s\n",SDL_GetError());

    window = SDL_CreateWindow(
            "Chessbox",
//            SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,
            300,100,
            SCREEN_WIDTH, SCREEN_HEIGHT,
            fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP:SDL_WINDOW_RESIZABLE);

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    FontManager::instance()->add("small","Inconsolata-Medium.ttf",10);
    FontManager::instance()->add("normal","Inconsolata-Medium.ttf",16);
    FontManager::instance()->add("large","Inconsolata-Medium.ttf",26);
    r.w = 100;
    r.h = 50;
    r2.w = r.w;
    r2.h = r.h;
    center.x = r.w / 2;
    center.y = r.h / 2;

    renderer = SDL_CreateRenderer(window, -1, 0);
    if(!renderer) {
        printf("rederer error %s\n",SDL_GetError());
    }
    SDL_SetRenderDrawColor(renderer, 128, 0, 0, 255);

    movesPanel=new MovesPanel("moves",480-200,480,200,800-320,board.rules());
    whiteClockText=new Label("whiteclock", 0, 480, 140, 25);
    whiteClockText->setFont(FontManager::instance()->font("large"));
    blackClockText=new Label("blackclock", 140, 480, 140, 25);
    blackClockText->setFont(FontManager::instance()->font("large"));

    logo.load(renderer,"assets/logo-sm.png",1,82+105/2,518+130/2);
    uistuff.push_back(&logo);
    movesBG.load(renderer,"assets/moves-bg1.png",1,480-100,480+320/2);
    uistuff.push_back(&movesBG);
    uistuff.push_back(whiteClockText);
    uistuff.push_back(blackClockText);

    SDL_Color whiteColor = {255,255,255};
    blackClockText->setColor(whiteColor);
    whiteClockText->setColor(whiteColor);
    whiteClockText->setText(" W 1:00:00");
    blackClockText->setText(" B 1:00:00");

    int ww=60,hh=60,xx=0,yy=0;
    AnimButton settingsButton("settings",renderer,"assets/button-gear.png",1,xx,yy);
    xx+=ww+10;
    AnimButton quitButton("power",renderer,"assets/button_power.png",1,xx,yy);
    xx+=ww+10;
    TextButton hintButton("hint","Hint",xx,yy,ww,hh);
    xx+=ww+10;
    TextButton inspectButton("inspect","Inspect",xx,yy,ww,hh);
    xx+=ww+10;


    xx=0;
    yy+=70;
    AnimButton fastbackButton("fastback",renderer,"assets/button-fastback.png",1,xx,yy);
    xx+=ww+10;
    AnimButton backButton("back",renderer,"assets/button-back.png",1,xx,yy);
    xx+=ww+10;
    AnimButton fwdButton("fwd",renderer,"assets/button-fwd.png",1,xx,yy);
    xx+=ww+10;
    AnimButton fastfwdButton("fastfwd",renderer,"assets/button-fastfwd.png",1,xx,yy);
    xx+=ww+10;

    board.loadPieces(renderer,"merida_new");
    buttonGroup.add(&settingsButton);
    buttonGroup.add(&quitButton);
    buttonGroup.add(&hintButton);
    buttonGroup.add(&inspectButton);
    buttonGroup.add(&fastbackButton);
    buttonGroup.add(&backButton);
    buttonGroup.add(&fwdButton);
    buttonGroup.add(&fastfwdButton);

    TextField tf("textfield",10,10,460,30);
    tf.setText("your name here");
    uistuff.push_back(&tf);

    running=true;
    while (running)
    {
        SDL_Event event;
        if (SDL_PollEvent(&event))
        {
//            printf("got event type %04X\n", event.type);
            switch (event.type)
            {
                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEBUTTONUP:
                case SDL_MOUSEMOTION:
                case SDL_MOUSEWHEEL:
                    processMouseEvent(&event);
                    break;
            case SDL_QUIT:
                running = false;
                break;
            case SDL_WINDOWEVENT:
//                printf("had a m_window event!!!\n");
                break;
            case SDL_KEYDOWN:
                if(SDL_SCANCODE_ESCAPE==event.key.keysym.scancode)
                    running = false;
                break;
            default:
                break;
            }
        }
        if(isInvalidated()) {
            validate();
            SDL_SetRenderDrawColor(renderer, 191, 37, 0, SDL_ALPHA_OPAQUE);
            SDL_RenderClear(renderer);

            board.draw(renderer);
            for (list<Component *>::iterator it = uistuff.begin(); it != uistuff.end(); it++) {
                (*it)->draw(renderer);
            }
            buttonGroup.draw(renderer);
            movesPanel->draw(renderer);

            SDL_RenderPresent(renderer);
        }
        Uint32 ticks = SDL_GetTicks();
        for (list<Component *>::iterator it = uistuff.begin(); it != uistuff.end(); it++) {
            (*it)->update(ticks);
        }
        buttonGroup.update(ticks);
        board.update(ticks);
        movesPanel->update(ticks);

        if(ticks>timer) {
            invalidate();
            timer=ticks+100;
        }

#if 1
//        if(!connector.isConnected() && attemptConnect) {
//            attemptConnect=false;
//            try {
//                printf("Connecting to controller at %s:%d\n",host,port);
//                connector.connect(host, port);
//                printf("Connected to controller\n");
//                quitButton.setChecked(true);
//            } catch(SocketInstanceException& e) {
//                printf("Not able to connect to %s:%d\n",host,port);
//            }
//        }
        if(connector.isConnected()) {
            char buffer[1024];
            if(connector.readline(buffer, sizeof(buffer))) {
                printf("Read from socket: %s\n",buffer);
                if(buffer[0]=='{') {
                    json j = json::parse(buffer);
                    if(j.contains("action")) {
                        string action = j["action"];
                        if(!action.compare("pieceUp")) {
                            string square=j["square"];
                            board.highlightSquare(board.toIndex(square.c_str()),true);
                            invalidate();
                        } else if(!action.compare("pieceDown")) {
                            string square=j["square"];
                            board.highlightSquare(board.toIndex(square.c_str()),false);
                            invalidate();
                        } else if(!action.compare("move")) {
                            ChessAction* a=new ChessAction(j);
                            ChessMove m = a->move(0);
                            printf("Move from=%s to=%s lan=%s\n",m.from(),m.to(),m.lan());
                            board.playMove(m.lan());
                            invalidate();
                        } else if(!action.compare("setposition")) {
                            string fen=j["fen"];
                            board.Forsyth(fen.c_str());
                            board.rules()->display_position();
                            movesPanel->clear();
                            invalidate();
                        }
                    }
                }

                if(buffer[0]=='W') {
                    connector.send("{\"action\":\"ping\"}\r\n");
                    connector.send("{\"action\":\"fen\"}\r\n");
                }
            }
        }
#endif
    }
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    TTF_Quit();
    SDL_Quit();
}


//Just some test stuff
void foo(void(*f)()) {
    printf("calling f\n");
    f();
}

class A {
public:
    void foo() {
        bar();
    }
    virtual void bar() {
        printf("A foobar\n");
    }
    virtual void cat() { printf("A cat\n"); }
};

class B : public A {
public:
    virtual void bar() {
        printf("B bar\n");
    }
    virtual void cat() { printf("B cat\n"); }
};

class Greet : public B {
public:
    string m_name;
    Greet(string name) : m_name(name) {}
    virtual void bar() {
        printf("Hello %s\n",m_name.c_str());
    }
};

void svgtest(const char* filename,bool fullscreen) {
    SDL_Window* m_window;
    SDL_Renderer* m_renderer;
    bool m_running;
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
        return;
    }
    m_window = SDL_CreateWindow(
            "Chessbox",
            300,100,
//            SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,
            SCREEN_WIDTH, SCREEN_HEIGHT,
            fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP:SDL_WINDOW_RESIZABLE);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    m_renderer = SDL_CreateRenderer(m_window, -1, 0);
    if(!m_renderer) {
        printf("m_renderer error %s\n",SDL_GetError());
    }

    SDL_RWops *src = SDL_RWFromFile(filename, "rb");
    SDL_Surface * surface = IMG_LoadSVG_RW(src);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(m_renderer,surface);
    SDL_Rect srcrect={0,0,0,0};
    SDL_QueryTexture(texture, NULL, NULL, &srcrect.w, &srcrect.h);
    SDL_Rect destrect={0,0,srcrect.w,srcrect.h};

    bool running=true;
    while(running) {
        SDL_Event event;
        if (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_WINDOWEVENT:
                    break;
                case SDL_KEYDOWN:
                    if(SDL_SCANCODE_ESCAPE==event.key.keysym.scancode)
                        running = false;
                    break;
                default:
                    break;
            }
        }
        SDL_SetRenderDrawColor(m_renderer, 128, 0, 0, 255);
        SDL_RenderClear(m_renderer);
        SDL_RenderCopyEx(m_renderer,texture, &srcrect, &destrect, 0, 0, SDL_FLIP_NONE);
        SDL_RenderPresent(m_renderer);
    }
    SDL_DestroyWindow(m_window);
    SDL_DestroyRenderer(m_renderer);
    SDL_Quit();


}

void add(MovesPanel& m,const char* s) {
    m.add(s);
}
void show(MovesPanel& m) {
    for(int i=0; i<17; i++) {
        printf("%02d %s\n",i,m.text(i));
    }
}
void scrolltest() {
    BoardRules rules;
    MovesPanel m("moves",0,0,100,100,&rules);
    int i;
    show(m);
    for(int i=0; i<20; i++) {
        snprintf(buffer,sizeof(buffer),"add %d",i);
        printf("%s (enter)\n",buffer);
        getchar();
        add(m,buffer);
        show(m);
    }
}

int main(int argc, char* argv[]) {
    printf("You may need to run 'export SDL_VIDEODRIVER=rpi'\n");
#if 0
    B b;
    b.foo();
    A* a = &b;
    a->foo();
    a->bar();

    Greet lee("Lee");
    Greet pauline("Pauline");
    Greet frank("Frank");
    
    lee.foo();
    a=&lee;
    a->foo();
    a->bar();

    printf("---\n");
    list<A*> ass;
    ass.push_back(&lee);
    ass.push_back(&pauline);
    ass.push_back(&frank);

    for (list<A *>::iterator it = ass.begin(); it != ass.end(); it++) {
        B* b = static_cast<B*>(*it);
        b->foo();
        b->cat();
    }

    return 1;
#endif

#if 0
    BoardRules rules;
    printf("history index=%d\n",rules.historyIndex());
    thc::Move mv;
    mv.NaturalIn(&rules,"d4");  //    mv.TerseIn(&rules, buffer);
    rules.PlayMove(mv);
    mv.NaturalIn(&rules,"d5");
    rules.PlayMove(mv);
    rules.display_position();
    mv = rules.historyAt(1);
    printf("move %s %s\n",mv.NaturalOut(&rules).c_str(),mv.TerseOut().c_str());
    mv = rules.historyAt(2);
    printf("move %s %s\n",mv.NaturalOut(&rules).c_str(),mv.TerseOut().c_str());
    printf("history index=%d\n",rules.historyIndex());
#endif

#if 0
    printf("Connecting\n");
    Connector c;
    c.connect("192.168.1.54",9999);
    char buf[]="123456789012345678901234567890123456789012345678901234567890";
    int n=0;
    while((n=c.readline(buf,10))==0)
        printf("waiting for data %lu\n",time(NULL));
    memset(buf,'z',sizeof(buf));
    n=c.readline(buf,10);
    memset(buf,'z',sizeof(buf));
    n=c.readline(buf,10);
    memset(buf,'z',sizeof(buf));
    n=c.readline(buf,10);
    memset(buf,'z',sizeof(buf));
    n=c.readline(buf,10);
    memset(buf,'z',sizeof(buf));
    n=c.readline(buf,10);
    memset(buf,'z',sizeof(buf));


    printf("Done\n");
#endif
#if 1
    int numdrivers, i, working;
    const char* drivername;

    if (SDL_Init(0) != 0) {
        printf("Error initializing SDL:  %s\n", SDL_GetError());
        return 1;
    }
    atexit(SDL_Quit);

    numdrivers = SDL_GetNumVideoDrivers();
    working = 0;

    for (i = 0; i < numdrivers; ++i) {
        drivername = SDL_GetVideoDriver(i);

        if (SDL_VideoInit(drivername) == 0) {
            SDL_VideoQuit();
            ++working;
            printf("Driver %s works.\n", drivername);
        }
        else {
            printf("Driver %s doesn't work.\n", drivername);
        }
    }

    printf("\n%d video drivers total (%d work)\n", numdrivers, working);

#endif
#if 0
    BoardRules rules;
    //1.d4 Nc6 2.Nf3 d5 3.Bf4 Bg4 4.h3 Bh5 5.Nbd2
    rules.playMove("d4");
    rules.playMove("Nc6");
    rules.playMove("Nf3");
    rules.playMove("d5");
    rules.playMove("Bf4");
    rules.playMove("Bg4");
    rules.playMove("h3");
    rules.playMove("Bh5");
    rules.playMove("Nbd2");
    rules.display_position();

    for(int i=1; i<rules.historyIndex(); i++) {
        Move mv=rules.historyAt(i);
        printf("%s %s\n",mv.TerseOut().c_str(),mv.NaturalOut(&rules).c_str());
    }
#endif
#if 1

    [](){}();   //cool lambda that does nothing, but is valid and C++ compiles

    bool fullscreen=false;
    for(int i=1; i<argc; i++) {
        if(!strcmp("-f",argv[i])) {
            fullscreen=true;
        } else if(!strcmp("-h",argv[i]) && i+1 <= argc) {
            strncpy(host,argv[++i],sizeof(host));
            NULL_TERMINATE(host, sizeof(host));
        }
    }
    if(argc>2) {
        strncpy(host,argv[1],sizeof(host));
        NULL_TERMINATE(host, sizeof(host));
    }
//    scrolltest();
//    coolSpot(fullscreen);
    ControllerGUI gui(host,port);
    gui.startGame(fullscreen);
#endif
//    svgtest("assets/chessbox-box.svg",false);
    return 0;
}
//todo lee settings should allow for piece choosing
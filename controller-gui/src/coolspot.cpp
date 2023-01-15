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
//-e "C:\Program Files\tarrasch-v3.12b-g\Engines\stockfish_8_x64.exe" -h 192.168.1.54
//-e "C:/Program Files (x86)/ShredderChess/Deep Shredder 13/EngineDeepShredder13UCIx64.exe" -h 192.168.1.54
#include <stdio.h>
#include <string.h>

#include <algorithm>
#include <list>
#include <stdlib.h>

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

char host[80]="127.0.0.1";
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
        printf("Event for %s\n",result->id());
        invalidate();
    }
}

void coolSpot(bool fullscreen) {
    SDL_Rect r;
    SDL_Rect r2;
    SDL_Point center;
    Uint32 timer=0;

    SDL_EnableScreenSaver();

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
                    connector.send("{\"action\":\"ping\"}");
                    connector.send("{\"action\":\"fen\"}");
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

    bool fullscreen=false;
    for(int i=1; i<argc; i++) {
        if(!strcmp("-f",argv[i])) {
            fullscreen=true;
        }
    }
//    scrolltest();
    coolSpot(fullscreen);
    return 0;
}
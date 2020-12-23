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

#include "Sprite.h"
#include "Button.h"
#include "Board.h"
#include "UIGroup.h"
#include "thc.h"
#include "Label.h"
#include "MovesPanel.h"
#include "Connector.h"

using namespace std;

const int FULL_SCREEN_MODE = SDL_WINDOW_RESIZABLE;
//const int FULL_SCREEN_MODE = SDL_WINDOW_FULLSCREEN_DESKTOP;
const int SCREEN_WIDTH = 480;
const int SCREEN_HEIGHT = 800;
//#define NUM_MOVES 16


list<Component*> uistuff;
UIGroup buttonGroup("buttons",0,670,280,130);
//UIGroup movesGroup("moves",480-200,480,200,800-320);

Connector connector;
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

void processMouseEvent(SDL_Event* event) {
    Component* result = buttonGroup.mouseEvent(event);
    if(result) {
        printf("Event for %s\n",result->id());
        if(!strcmp(result->id(),"power")) {
            connector.connect("192.168.1.54",9999);
            printf("Connected to controller\n");
        } else if(!strcmp(result->id(),"settings")) {
            whiteClockText->setText("XXX");
        } else if(!strcmp(result->id(),"fwd")) {
            thc::Move mv;
            mv.NaturalIn(board.rules(),gameMoves[gameMovesIndex++]);
            movesPanel->add(mv);
        } else if(!strcmp(result->id(),"back")) {
            const char* fen = "rnbqkb1r/ppp1pppp/5n2/3p4/3P1B2/4P3/PPP2PPP/RN1QKBNR b KQkq - 0 3";
            board.Forsyth(fen);
        } else if(!strcmp(result->id(),"hint")) {
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

    //was processing the components here, then using a lamba. Will see about using a lambda again later.
//    for(list<Component*>::iterator it=buttonGroup.begin(); it != buttonGroup.end(); ++it) {
//        Button* b = static_cast<Button*>(*it);
//        b->mouseEvent(event,[](Button* b){printf("button %s was clicked size=%dx%d\n",b->id(),b->rect()->w,b->rect()->h);});  //called method signature is //bool Button::mouseEvent(SDL_Event* event,void(*f)(Button* b));
//    }
}

void coolSpot(const char* assets) {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Rect r;
    SDL_Rect r2;
    SDL_Point center;

    SDL_EnableScreenSaver();

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
        return;
    }

    window = SDL_CreateWindow(
            "Chessbox",
//            SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,
            300,100,
            SCREEN_WIDTH, SCREEN_HEIGHT,
            FULL_SCREEN_MODE);

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    TTF_Init();
    r.w = 100;
    r.h = 50;
    r2.w = r.w;
    r2.h = r.h;
    center.x = r.w / 2;
    center.y = r.h / 2;

    renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_SetRenderDrawColor(renderer, 128, 0, 0, 255);

    movesPanel = new MovesPanel("moves",480-200,480,200,800-320,board.rules());
    whiteClockText = new Label("whiteclock", 0, 480, 140, 25, 26);
    blackClockText = new Label("blackclock", 140, 480, 140, 25, 26);

    logo.load(renderer,"assets/logo-sm.png",1,82,518);
    uistuff.push_back(&logo);
    movesBG.load(renderer,"assets/moves-bg1.png",1,480-200,480);
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

    board.loadPieces(renderer,"spatial");
    buttonGroup.add(&settingsButton);
    buttonGroup.add(&quitButton);
    buttonGroup.add(&hintButton);
    buttonGroup.add(&inspectButton);
    buttonGroup.add(&fastbackButton);
    buttonGroup.add(&backButton);
    buttonGroup.add(&fwdButton);
    buttonGroup.add(&fastfwdButton);

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
//                printf("had a window event!!!\n");
                break;
            case SDL_KEYDOWN:
                running = false;
                break;
            default:
                break;
            }
        }
        SDL_SetRenderDrawColor(renderer, 191, 37, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);

        board.draw(renderer);
        for (list<Component *>::iterator it = uistuff.begin(); it != uistuff.end(); it++) {
            (*it)->draw(renderer);
        }
        buttonGroup.draw(renderer);
        movesPanel->draw(renderer);

        SDL_RenderPresent(renderer);

        Uint32 ticks = SDL_GetTicks();
        for (list<Component *>::iterator it = uistuff.begin(); it != uistuff.end(); it++) {
            (*it)->update(ticks);
        }
        buttonGroup.update(ticks);
        board.update(ticks);
        movesPanel->update(ticks);

        if(connector.isConnected()) {
            char buffer[1024];
            if(connector.readline(buffer, sizeof(buffer))) {
                printf("Read from socket: %s\n",buffer);
                if(buffer[0]=='W')
                    connector.send("{\"action\":\"ping\"}\r\n");
            }
        }
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

int main(int argc, char* argv[]) {
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
    char assets[255]="assets";
    if(argc>2) {
        strncpy(assets,argv[1],sizeof(assets));
    }

    [](){}();   //cool lambda that does nothing, but is valid and C++ compiles

    coolSpot(assets);
#endif
    return 0;
}
//todo lee settings should allow for piece choosing
//todo lee setting to inspect mode
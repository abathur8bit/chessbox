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

#include "Sprite.h"
#include "Button.h"
#include "Board.h"
#include "UIGroup.h"
#include "thc.h"
#include "Label.h"

using namespace std;

const int FULL_SCREEN_MODE = SDL_WINDOW_RESIZABLE;
//const int FULL_SCREEN_MODE = SDL_WINDOW_FULLSCREEN_DESKTOP;
const int SCREEN_WIDTH = 480;
const int SCREEN_HEIGHT = 800;


list<Component*> uistuff;
UIGroup buttonGroup("buttons",0,670,280,130);
UIGroup movesGroup("moves",480-200,480,200,800-320);

bool running=false;
Board board(0,0,480,480);
Sprite logo("logo");
Sprite movesBG("moves-bg1");
Label* blackPlayerText;
Label* whitePlayerText;

void processMouseEvent(SDL_Event* event) {
    Component* result = buttonGroup.mouseEvent(event);
    if(result) {
        printf("Event for %s\n",result->id());
        if(!strcmp(result->id(),"quit")) {
            printf("User wants to quit\n");
            running=false;
        } else if(!strcmp(result->id(),"settings")) {
            whitePlayerText->setText("XXX");
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
            200,100,
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

    blackPlayerText = new Label("black", 0, 480, 140, 25);
    whitePlayerText = new Label("white", 140, 480, 140, 25);

    logo.load(renderer,"assets/logo-sm.png",1,82,518);
    uistuff.push_back(&logo);
    movesBG.load(renderer,"assets/moves-bg1.png",1,480-200,480);
    uistuff.push_back(&movesBG);
    uistuff.push_back(whitePlayerText);
    uistuff.push_back(blackPlayerText);

    SDL_Color whiteColor = {255,255,255};
    blackPlayerText->setColor(whiteColor);
    whitePlayerText->setColor(whiteColor);
    blackPlayerText->setText("B: 1:00:00");
    whitePlayerText->setText("    W: 1:00:00");

    int ww=60,hh=60,xx=0,yy=0;
    AnimButton settingsButton("settings",renderer,"assets/button-gear.png",1,xx,yy);
    xx+=ww+10;
    AnimButton quitButton("quit",renderer,"assets/button_power.png",1,xx,yy);
    xx+=ww+10;
    TextButton hintButton("hint","Hint",xx,yy,ww,hh);
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

        SDL_RenderPresent(renderer);

        Uint32 ticks = SDL_GetTicks();
        for (list<Component *>::iterator it = uistuff.begin(); it != uistuff.end(); it++) {
            (*it)->update(ticks);
        }
        buttonGroup.update(ticks);
        board.update(ticks);

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

    char assets[255]="assets";
    if(argc>2) {
        strncpy(assets,argv[1],sizeof(assets));
    }

    [](){}();   //cool lambda that does nothing, but is valid and C++ compiles

    coolSpot(assets);

    return 0;
}
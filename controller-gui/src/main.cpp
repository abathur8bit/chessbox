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

using namespace std;

const int FULL_SCREEN_MODE = SDL_WINDOW_RESIZABLE;
//const int FULL_SCREEN_MODE = SDL_WINDOW_FULLSCREEN_DESKTOP;
const int SCREEN_WIDTH = 480;
const int SCREEN_HEIGHT = 800;


list<Button*> buttons;
UIGroup buttonGroup("buttons",0,699,480,100);

void processMouseEvent(SDL_Event* event) {
    buttonGroup.mouseEvent(event);
//    for(list<Component*>::iterator it=buttonGroup.begin(); it != buttonGroup.end(); ++it) {
//        Button* b = static_cast<Button*>(*it);
//        b->mouseEvent(event,[](Button* b){printf("button %s was clicked size=%dx%d\n",b->id(),b->rect()->w,b->rect()->h);});
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
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            SCREEN_WIDTH, SCREEN_HEIGHT,
            FULL_SCREEN_MODE);

    TTF_Init();
    r.w = 100;
    r.h = 50;
    r2.w = r.w;
    r2.h = r.h;
    center.x = r.w / 2;
    center.y = r.h / 2;

    renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_SetRenderDrawColor(renderer, 128, 0, 0, 255);

    const int max = 25;
    Sprite cool[max];
    int x = 0;
    int y = 100;
    char filename[255];
    snprintf(filename, sizeof(filename), "%s/coolspot_fingersnap.png", assets);
    for (int i = 0; i < max; i++)
    {
        SDL_Texture* t = cool[i].load(renderer, filename, 10, x, y);
        if (!t) {
            printf("Couldn't initialize SDL: %s\n", SDL_GetError());

        }
        cool[i].setFrame(i % 10);
        x += cool[i].widthSource() * SCALE;
        if (x > SCREEN_WIDTH - cool[i].widthSource()) {
            x = 0;
            y += cool[i].heightSource() * SCALE;
        }
    }
    int ww=60,hh=60,xx=0,yy=0;
    TextButton quitButton("quit","Quit", xx, yy, ww, hh);
    xx+=ww;
    TextButton pingButton("ping","Ping",xx,yy,ww,hh);
    xx+=ww;
    AnimButton imageButton("image",renderer,"assets/button-twoplayer-whiteblack.png",1,xx,yy);
    imageButton.setChecked(true);
    xx+=ww;
    snprintf(filename,sizeof(filename),"%s/coolspot_dusting.png",assets);
    AnimButton animButton("anim",renderer,filename,10,xx,yy);
    xx+=ww;

    Board board(0,0,480,480);
    buttonGroup.add(&quitButton);
    buttonGroup.add(&quitButton);
    buttonGroup.add(&pingButton);
    buttonGroup.add(&animButton);
    buttonGroup.add(&imageButton);

    buttons.push_back(&quitButton);
    buttons.push_back(&pingButton);
    buttons.push_back(&animButton);
    buttons.push_back(&imageButton);

    bool running = true;
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
                printf("had a window event!!!\n");
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
        for (int i = 0; i < max; i++)
        {
            cool[i].draw(renderer);
        }
        buttonGroup.draw(renderer);
//        for(std::list<Button*>::iterator it=buttons.begin(); it != buttons.end(); ++it) {
//            (*it)->draw(renderer);
//        }

        SDL_RenderPresent(renderer);

        Uint32 ticks = SDL_GetTicks();
        for (int i = 0; i < max; i++)
        {
            cool[i].update(ticks);
        }
        buttonGroup.update(ticks);
//        for(std::list<Button*>::iterator it=buttons.begin(); it != buttons.end(); ++it) {
//            (*it)->update(ticks);
//        }
        board.update(ticks);

    }
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    TTF_Quit();
    SDL_Quit();
}


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

    coolSpot(assets);

    return 0;
}
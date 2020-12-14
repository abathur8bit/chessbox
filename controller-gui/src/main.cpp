#include <stdio.h>
#include <string.h>
#include <algorithm>
#include <SDL.h>

#include "AnimSprite.h"
#include "Button.h"
#include "Board.h"

using namespace std;

const int FULL_SCREEN_MODE = SDL_WINDOW_RESIZABLE;
//const int FULL_SCREEN_MODE = SDL_WINDOW_FULLSCREEN_DESKTOP;
const int SCREEN_WIDTH = 480;
const int SCREEN_HEIGHT = 800;




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
            "ChessBox",
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
    AnimSprite cool[max];
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
    Button button("QUIT",10,10,50,30);
    AnimSprite coolButton;
    snprintf(filename,sizeof(filename),"%s/button_power.png",assets);
    coolButton.load(renderer,filename, 1, 100, 10);
    AnimButton animButton("quit",coolButton);
    Board board(0,0,480,480);

    button.setListener([](){printf("In button lambda\n");});
    bool running = true;
    while (running)
    {
        SDL_Event event;
        if (SDL_PollEvent(&event))
        {
            printf("got event type %04X\n", event.type);
            switch (event.type)
            {
                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEBUTTONUP:
                case SDL_MOUSEMOTION:
                case SDL_MOUSEWHEEL:
                    button.mouseEvent(event,[](){printf("button was clicked\n");});
                    animButton.mouseEvent(event,[](){printf("anim button was clicked\n");});
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
        button.draw(renderer);
//        animButton.draw(renderer);

        SDL_RenderPresent(renderer);

        Uint32 ticks = SDL_GetTicks();
        for (int i = 0; i < max; i++)
        {
            cool[i].update(ticks);
        }

        button.update(ticks);
//        animButton.update(ticks);
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
int main(int argc, char* argv[]) {
    char assets[255]="assets";
    if(argc>2) {
        strncpy_s(assets,argv[1],sizeof(assets));
    }

//    foo([](){printf("from lambda\n");});

    coolSpot(assets);

    return 0;
}
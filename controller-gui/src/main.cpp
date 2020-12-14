#include <stdio.h>

#include <SDL.h>

#include "AnimSprite.h"
#include "Button.h"

using namespace std;

const int FULL_SCREEN_MODE = SDL_WINDOW_RESIZABLE;
//const int FULL_SCREEN_MODE = SDL_WINDOW_FULLSCREEN_DESKTOP;
const int SCREEN_WIDTH = 480;
const int SCREEN_HEIGHT = 800;





void coolSpot() {
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
    int y = 0;
    for (int i = 0; i < max; i++)
    {
        SDL_Texture* t = cool[i].load(renderer, "coolspot_fingersnap.png", 10, x, y);
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
    Button button("quit",10,SCREEN_HEIGHT-10-30,50,30);
    AnimSprite coolButton;
    coolButton.load(renderer,"coolspot_fingersnap.png", 10, 100, 500);
    AnimButton animButton("quit",coolButton);

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
                    button.mouseEvent(event);
                    animButton.mouseEvent(event);
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

        for (int i = 0; i < max; i++)
        {
            cool[i].draw(renderer);
        }
        button.draw(renderer);
        animButton.draw(renderer);

        SDL_Rect rect = { 10,10,50,50 };
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderDrawRect(renderer, &rect);

        SDL_RenderPresent(renderer);

        Uint32 ticks = SDL_GetTicks();
        for (int i = 0; i < max; i++)
        {
            cool[i].update(ticks);
        }
        button.update(ticks);
        animButton.update(ticks);
    }
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
}


int main(int argc, char* argv[]) {
    coolSpot();
    return 0;
}
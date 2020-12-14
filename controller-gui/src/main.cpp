#include <string>
#include <stdio.h>
#include <SDL.h>
#include <SDL_image.h>

#include "AnimSprite.h"

using namespace std;

const int FULL_SCREEN_MODE = SDL_WINDOW_RESIZABLE;
//const int FULL_SCREEN_MODE = SDL_WINDOW_FULLSCREEN_DESKTOP;
const int SCREEN_WIDTH = 480;  
const int SCREEN_HEIGHT = 800;

bool coolSpot() {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    SDL_Event event;
    SDL_Rect r;
    SDL_Rect r2;
    SDL_Point center;
    SDL_Joystick* joystick = nullptr;

    SDL_EnableScreenSaver();

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
        return false;
    }

//    if (!SDL_WasInit(SDL_INIT_JOYSTICK))
//    {
//        printf("initializing joystick %08X", joystick);
//        SDL_InitSubSystem(SDL_INIT_JOYSTICK);
//        int joystickCount = SDL_NumJoysticks();
//        printf("joysticks: %d", joystickCount);
//        joystick = SDL_JoystickOpen(0);
//        if (joystickCount == 2)
//        {
//            SDL_JoystickOpen(1);
//        }
//        SDL_JoystickEventState(SDL_ENABLE);
//    }
//    else
//    {
//        printf("Joystick already initialized");
//    }
//
    window = SDL_CreateWindow("ChessBox",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH, SCREEN_HEIGHT,
        SDL_WINDOW_RESIZABLE);

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
//        SDL_Texture* t = cool[i].load(renderer, "coolspot_dusting.png", 10, x, y);
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

    bool running = true;
    while (running)
    {
        SDL_Event event;
        if (SDL_PollEvent(&event))
        {
                printf("got event type %04X\n", event.type);
                switch (event.type)
                {
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
        SDL_SetRenderDrawColor(renderer, 128, 128, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);

        for (int i = 0; i < max; i++)
        {
            cool[i].draw(renderer);
        }

        SDL_Rect rect = { 10,10,50,50 };
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderDrawRect(renderer, &rect);

        SDL_RenderPresent(renderer);

        Uint32 ticks = SDL_GetTicks();
        for (int i = 0; i < max; i++)
        {
            cool[i].update(ticks);
        }
    }
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);

    SDL_Quit();

}


int main(int argc, char* argv[]) {
    coolSpot();

    return 0;
}
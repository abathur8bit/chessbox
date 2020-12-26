//
// Created by patte on 12/24/2020.
//

#include "ControllerGUI.h"
#include "UIGroup.h"

const int FULL_SCREEN_MODE = SDL_WINDOW_RESIZABLE;
//const int FULL_SCREEN_MODE = SDL_WINDOW_FULLSCREEN_DESKTOP;
const int SCREEN_WIDTH = 320;
const int SCREEN_HEIGHT = 240;

ControllerGUI::ControllerGUI(bool fullscreen,const char* host,unsigned short port)
    : m_window(nullptr),
    m_renderer(nullptr),
    m_running(false),
    m_fullscreen(fullscreen),
    m_host(host),
    m_port(port)
{

}

void ControllerGUI::startGame() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
        return;
    }
    m_window = SDL_CreateWindow(
            "Chessbox",
            SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,
//            300,100,
            SCREEN_WIDTH, SCREEN_HEIGHT,
            FULL_SCREEN_MODE);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    m_renderer = SDL_CreateRenderer(m_window, -1, 0);
    if(!m_renderer) {
        printf("m_renderer error %s\n",SDL_GetError());
    }
    UIGroup group("group1",0,0,320,240);
    Sprite logo("logo");
    logo.load(m_renderer, "assets/logo-sm.png", 320 / 2, 240 / 2);
    Button button("button",0,0,100,60);
    group.add(&button);
    group.add(&logo);
    SDL_Rect rect = {10,10,300,220};
    m_running=true;
    while(m_running) {
        SDL_Event event;
        if (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEBUTTONUP:
                case SDL_MOUSEMOTION:
                case SDL_MOUSEWHEEL:
                    button.mouseEvent(&event);
                    break;

                case SDL_QUIT:
                    m_running = false;
                    break;
                case SDL_WINDOWEVENT:
//                printf("had a m_window event!!!\n");
                    break;
                case SDL_KEYDOWN:
                    m_running = false;
                    break;
                default:
                    break;
            }
        }
        SDL_SetRenderDrawColor(m_renderer, 128, 0, 0, 255);
        SDL_RenderClear(m_renderer);
        SDL_SetRenderDrawColor(m_renderer, 191, 37, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderFillRect(m_renderer, &rect);
        group.draw(m_renderer);
        SDL_RenderPresent(m_renderer);

        Uint32 ticks = SDL_GetTicks();
        logo.update(ticks);
        button.update(ticks);
    }
    SDL_DestroyWindow(m_window);
    SDL_DestroyRenderer(m_renderer);
    SDL_Quit();
}
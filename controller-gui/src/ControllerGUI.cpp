//
// Created by patte on 12/24/2020.
//

#include "ControllerGUI.h"
#include "UIGroup.h"

const int SCREEN_WIDTH = 480;
const int SCREEN_HEIGHT = 800;

ControllerGUI::ControllerGUI(const char* host,unsigned short port)
    : m_window(nullptr),
    m_renderer(nullptr),
    m_running(false),
    m_host(host),
    m_port(port)
{

}

void ControllerGUI::startGame(bool fullscreen) {
    SDL_Color background={189,37,0};
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
    m_renderer = SDL_CreateRenderer(m_window, -1, 0);
    if(!m_renderer) {
        printf("m_renderer error %s\n",SDL_GetError());
    }
    UIGroup group("group1",0,0,320,240);
    Sprite logo("logo");
    logo.load(m_renderer, "assets/logo-sm.png", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
    SDL_Rect rect = {10,10,SCREEN_WIDTH-20,SCREEN_HEIGHT-20};
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
        SDL_SetRenderDrawColor(m_renderer, background.r, background.g, background.b, 255);
        SDL_RenderClear(m_renderer);
        group.draw(m_renderer);
        SDL_RenderPresent(m_renderer);

        Uint32 ticks = SDL_GetTicks();
        logo.update(ticks);
    }
    SDL_DestroyWindow(m_window);
    SDL_DestroyRenderer(m_renderer);
    SDL_Quit();
}
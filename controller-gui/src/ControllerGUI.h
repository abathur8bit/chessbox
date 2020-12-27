//
// Created by patte on 12/24/2020.
//

#ifndef CONTROLLER_GUI_CONTROLLERGUI_H
#define CONTROLLER_GUI_CONTROLLERGUI_H

#include <SDL.h>
#include "Sprite.h"
#include "Button.h"


class ControllerGUI {
public:
    ControllerGUI(const char* host,unsigned short port);
    void startGame(bool fullscreen);
    void processMouseEvent(SDL_Event* event);
    void saveGame();

protected:
    SDL_Window* m_window;
    SDL_Renderer* m_renderer;
    bool m_running;
    bool m_fullscreen;
    string m_host;
    unsigned short m_port;
};


#endif //CONTROLLER_GUI_CONTROLLERGUI_H

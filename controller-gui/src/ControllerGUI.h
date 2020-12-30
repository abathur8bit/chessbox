//
// Created by patte on 12/24/2020.
//

#ifndef CONTROLLER_GUI_CONTROLLERGUI_H
#define CONTROLLER_GUI_CONTROLLERGUI_H

#include <SDL.h>
#include "Sprite.h"
#include "Button.h"
#include "Window.h"
#include "Board.h"
#include "MovesPanel.h"
#include "Connector.h"
#include "UCIClient.h"

class ControllerGUI : Window {
public:
    ControllerGUI(bool fullscreen,const char* host,unsigned short port,const char* engine);
    ~ControllerGUI();
    void startGame();
    void saveGame();
    void initComponents();
    virtual void processButtonClicked(Button* c);
    virtual void update(long ticks);
    void connectController();
    void disconnectController();
    void processJson(const char* buffer);
protected:
    SDL_Window* m_window;
    SDL_Renderer* m_renderer;
    bool m_running;
    bool m_fullscreen;
    string m_host;
    unsigned short m_port;
    Board* m_board;
    BoardRules m_rules;
    MovesPanel* m_movesPanel;
    Connector* m_connector;
    char m_buffer[1024];
    UCIClient m_uci;
};


#endif //CONTROLLER_GUI_CONTROLLERGUI_H

//
// Created by patte on 12/14/2020.
//

#ifndef CONTROLLER_GUI_BUTTON_H
#define CONTROLLER_GUI_BUTTON_H

#include <string>
#include <SDL.h>
#include <SDL_image.h>

#include "AnimSprite.h"

using namespace std;

#define BUTTON_STATE_NORMAL 0
#define BUTTON_STATE_MOUSE_OVER 1
#define BUTTON_STATE_MOUSE_DOWN 2
#define BUTTON_STATE_MOUSE_UP   3


class Button {
protected:
    Uint32 m_delay;
    SDL_Rect m_sourceRect;
    int m_state;

public:
    Button(string id,int x,int y,int w,int h) {
        m_sourceRect = {x,y,w,h};
        m_state = 0;
    }
    void update(int ticks){}
    void draw(SDL_Renderer* renderer);
    bool mouseEvent(SDL_Event event);
};

class AnimButton : public Button {
protected:
    AnimSprite m_sprite;
public:
    AnimButton(string id,AnimSprite sprite);
    void update(int ticks);
    void draw(SDL_Renderer* renderer);
};

#endif //CONTROLLER_GUI_BUTTON_H

//
// Created by patte on 12/14/2020.
//

#ifndef CONTROLLER_GUI_BOARD_H
#define CONTROLLER_GUI_BOARD_H

#include <string>
#include <SDL.h>
#include <SDL_image.h>

#include "AnimSprite.h"

using namespace std;

class Board {
protected:
    SDL_Rect m_rect;
    SDL_Color m_blackColor;
    SDL_Color m_whiteColor;
public:
    Board(int x,int y,int w,int h);
    void update(int ticks);
    void draw(SDL_Renderer* renderer);
};


#endif //CONTROLLER_GUI_BOARD_H

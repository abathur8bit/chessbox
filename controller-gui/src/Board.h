//
// Created by patte on 12/14/2020.
//

#ifndef CONTROLLER_GUI_BOARD_H
#define CONTROLLER_GUI_BOARD_H

#include <string>
#include <SDL.h>
#include <SDL_image.h>

#include "thc.h"

#include "Sprite.h"
#include "Component.h"

using namespace std;

class BoardRules : public thc::ChessRules {
public:
    char pieceAt(int i) {
        return squares[i];
    }

    /**
     * Returns true if the move is a checking move (ends with '+'. Note
     * you should do the check before calling PlayMove.
     *
     * @param mv Move before it is played.
     * @return true if it is a check, false otherwise.
     */
    bool isCheck(thc::Move mv) {
        return mv.NaturalOut(this).find_first_of('+') == string::npos ? false:true;
    }
};

class Board : public Component {
protected:
    Sprite* m_pieces[16];
    SDL_Rect m_rectSquare;
    SDL_Color m_blackColor;
    SDL_Color m_whiteColor;
    BoardRules m_rules;
public:
    Board(int x,int y,int w,int h);
    virtual void draw(SDL_Renderer* renderer);
    void loadPieces(SDL_Renderer* renderer,const char* setName);
};


#endif //CONTROLLER_GUI_BOARD_H

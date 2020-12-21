/* *****************************************************************************
 * Created by Lee Patterson 12/21/2020
 *
 * Copyright 2019 Lee Patterson <https://github.com/abathur8bit>
 *
 * You may use and modify at will. Please credit me in the source.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * ******************************************************************************/

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

    thc::Move historyAt(unsigned char index) {
        return history[index];  //todo lee Seems like a bad idea to limit history to only 256 moves, should use a list.
    }

    unsigned char historyIndex() {return history_idx;}

    void display_position()
    {
        std::string fen = ForsythPublish();
        std::string s = ToDebugStr();
        printf("FEN = %s\n", fen.c_str());
        printf("Position = %s\n", s.c_str());
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

#define NUM_SQUARES 64
#define NUM_PIECES 12
class Board : public Component {
public:
    Board(int x,int y,int w,int h);
    virtual void draw(SDL_Renderer* renderer);
    void loadPieces(SDL_Renderer* renderer,const char* setName);
    void Forsyth(const char* fen);
    void highlightSquare(int square,bool highlight);
    bool isHighlighted(int square) {return m_highlight[square];}
    BoardRules* rules() {return &m_rules;}

protected:
    void drawSquares(SDL_Renderer* r);
    void drawPieces(SDL_Renderer* r);

    Sprite* m_pieces[NUM_PIECES];
    bool m_highlight[NUM_SQUARES];
    SDL_Rect m_rectSquare;
    SDL_Color m_blackColor;
    SDL_Color m_whiteColor;
    BoardRules m_rules;
};


#endif //CONTROLLER_GUI_BOARD_H

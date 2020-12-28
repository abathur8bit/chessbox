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
using namespace thc;



#define NUM_SQUARES 64
#define NUM_PIECES 12
#define SAN_BUF_SIZE 6      ///< Minimum buffer size to hold a san or long san move. Something like long "h7h8q" or san "h8=Q+"

#include "BoardRules.h"

/** Draws the checker pattern of a chess board, and has coordinate conversions. */
class Board : public Component {
public:
    const char* rowNames="87654321";
    const char* colNames="abcdefgh";
    Board(int x,int y,int w,int h);
    virtual void draw(SDL_Renderer* renderer);
    void loadPieces(SDL_Renderer* renderer,const char* setName);
    void Forsyth(const char* fen);
    void highlightSquare(int square,bool highlight);
    bool isHighlighted(int square) {return m_highlight[square];}
    BoardRules* rules() {return &m_rules;}
    void playMove(const char* sanLong);

    /** Returns a string like "a1" give a col='a' and row='1'. */
    char* toMove(char* buffer, size_t n, char col, char row) {
        snprintf(buffer, n, "%c%c", col, row);
        return buffer;
    }
    /** Returns a string like "a1" given a square index like 56 (lower left corner). */
    char* toMove(char* buffer,size_t n,int index) {
        snprintf(buffer,n,"%c%c",toCol(index),toRow(index));
        return buffer;
    }

    /** Return the letter character of the column the index points to. The column should display before the row. */
    char toCol(int index) {
        int y = index/8;
        int x = index-y*8;
        return colNames[x];
    }

    /** Returns the number character of the row the index points to. You should use the column, then row. */
    char toRow(int index) {
        int y = index/8;
        int x = index-y*8;
        return rowNames[y];
    }

    /** Convert a string like "a1" (n)ot case sensitive) into an index like 56, or "a8" to 0. Top left corner is 0, bottom right is 63. */
    int toIndex(const char* square) {
        int col=tolower(square[0])-'a';
        int row=8-(square[1]-'0');
        return row*8+col;
    }

    int toIndex(char col,char row) {
        char buffer[SAN_BUF_SIZE];
        toMove(buffer,sizeof(buffer),col,row);
        return toIndex(buffer);
    }

    void fromTo(const char* sanLong,char* from,char* to) {
        from[0]=sanLong[0];
        from[1]=sanLong[1];
        from[2]='\0';
        to[0]=sanLong[2];
        to[1]=sanLong[3];
        to[2]='\0';
    }

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

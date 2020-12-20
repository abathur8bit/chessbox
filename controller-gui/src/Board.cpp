//
// Created by patte on 12/14/2020.
//

#include "Board.h"

Board::Board(int x, int y, int w, int h) : Component("board",x,y,w,h),m_rules() {
    m_rectSquare = {x, y, w / 8, h / 8};
    m_whiteColor = {0x4B,0x99,0xC5,0xFF};
    m_blackColor = {0x00,0x6A,0xA6,0xFF};
    for(int i=0; i<NUM_PIECES; i++)
        m_pieces[i]=nullptr;
    for(int i=0; i<NUM_SQUARES; i++)
        m_highlight[i]=false;
}

void Board::loadPieces(SDL_Renderer* renderer,const char* setName) {
    char buffer[255];
    const char* piece[NUM_PIECES]= {"bk","bq","br","bb","bn","bp","wk","wq","wr","wb","wn","wp"};
    for(int i=0; i<NUM_PIECES; i++) {
        snprintf(buffer, sizeof(buffer), "assets/pieces/%s/%s.png", setName, piece[i]);
        m_pieces[i] = new Sprite();
        if(!m_pieces[i]->load(renderer, buffer, 0, 0)) {
            printf("failed to load %s\n",buffer);
        } else {
            printf("loaded %s\n", buffer);
        }
    }
}

void Board::draw(SDL_Renderer *renderer) {
    drawSquares(renderer);
    drawPieces(renderer);
}

void Board::drawSquares(SDL_Renderer *renderer) {
    int white=1;
    int i=0;
    SDL_Color color;
    for(int y=0; y<8; y++) {
        for(int x=0; x<8; x++) {
            int xx= m_rectSquare.x + m_rectSquare.w * x;
            int yy= m_rectSquare.y + m_rectSquare.h * y;
            SDL_Rect r = {xx, yy, m_rectSquare.w, m_rectSquare.h};
            if(white) {
                color = m_whiteColor;
                white=0;
            } else {
                color = m_blackColor;
                white=1;
            }
            if(m_highlight[i]) {
                color.r+=100;
                color.g+=100;
                color.b+=100;
            }
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
            SDL_RenderFillRect(renderer, &r);
            i++;
        }
        white=!white;
    }
}

void Board::drawPieces(SDL_Renderer *renderer) {
    int i=0;
    SDL_Rect dest = {0,0,m_rectSquare.w,m_rectSquare.h};
    for(int y=0; y<8; y++) {
        for (int x = 0; x < 8; x++) {
            Sprite *piece = nullptr;
            char p = m_rules.pieceAt(i++);
            switch (p) {
                case 'k':
                    piece = m_pieces[0];
                    break;
                case 'q':
                    piece = m_pieces[1];
                    break;
                case 'r':
                    piece = m_pieces[2];
                    break;
                case 'b':
                    piece = m_pieces[3];
                    break;
                case 'n':
                    piece = m_pieces[4];
                    break;
                case 'p':
                    piece = m_pieces[5];
                    break;
                case 'K':
                    piece = m_pieces[6];
                    break;
                case 'Q':
                    piece = m_pieces[7];
                    break;
                case 'R':
                    piece = m_pieces[8];
                    break;
                case 'B':
                    piece = m_pieces[9];
                    break;
                case 'N':
                    piece = m_pieces[10];
                    break;
                case 'P':
                    piece = m_pieces[11];
                    break;
            }
            if(piece) {
                dest.x=x*dest.w;
                dest.y=y*dest.h;
                piece->draw(renderer,&dest);
            }
        }
    }
}

void Board::Forsyth(const char *fen) {
    m_rules.Forsyth(fen);
}

void Board::highlightSquare(int square, bool highlight) {
    m_highlight[square]=highlight;
}
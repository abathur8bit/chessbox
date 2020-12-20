//
// Created by patte on 12/14/2020.
//

#include "Board.h"

Board::Board(int x, int y, int w, int h) : Component("board",x,y,w,h),m_rules() {
    m_rectSquare = {x, y, w / 8, h / 8};
    m_whiteColor = {0x4B,0x99,0xC5,0xFF};
    m_blackColor = {0x00,0x6A,0xA6,0xFF};
    for(int i=0; i<16; i++)
        m_pieces[i]=nullptr;
}

void Board::loadPieces(SDL_Renderer* renderer,const char* setName) {
    char buffer[255];
    const char* piece[12]= {"bk","bq","br","bb","bn","bp",
                            "wk","wq","wr","wb","wn","wp"};

    int w=60,h=60;
    int i=0;
    int x=0,y=0;
    for(int i=0; i<12; i++) {
//    for(int y=0; y<8; y++) {
//        for(int x=0; x<8; x++) {
        snprintf(buffer, sizeof(buffer), "assets/pieces/%s/%s.png", setName, piece[i]);
        m_pieces[i] = new Sprite();
        m_pieces[i]->load(renderer, buffer, 0, 0);
        printf("loading %s x=%d y=%d\n", buffer,x,y);
        if(x+w>=480 || (i==5)) {
            x=0;
            y+=h;
        } else {
            x+=w;
        }
//        i++;
//        }
//    }
    }
}

void Board::draw(SDL_Renderer *renderer) {
    int white=1;
    for(int y=0; y<8; y++) {
        for(int x=0; x<8; x++) {
            int xx= m_rectSquare.x + m_rectSquare.w * x;
            int yy= m_rectSquare.y + m_rectSquare.h * y;
            SDL_Rect r = {xx, yy, m_rectSquare.w, m_rectSquare.h};
            if(white) {
                SDL_SetRenderDrawColor(renderer, m_whiteColor.r, m_whiteColor.g, m_whiteColor.b, m_whiteColor.a);
                white=0;
            } else {
                SDL_SetRenderDrawColor(renderer,m_blackColor.r,m_blackColor.g,m_blackColor.b,m_blackColor.a);
                white=1;
            }
            SDL_RenderFillRect(renderer, &r);
        }
        white=!white;
    }

    int i=0;
    SDL_Rect dest = {0,0,60,60};
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
                dest.x=x*60;
                dest.y=y*60;
                piece->draw(renderer,&dest);
            }
        }
    }
}

void Board::Forsyth(const char *fen) {
    m_rules.Forsyth(fen);
}
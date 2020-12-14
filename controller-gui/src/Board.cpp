//
// Created by patte on 12/14/2020.
//

#include "Board.h"

Board::Board(int x, int y, int w, int h) {
    m_rect = {x,y,w/8,h/8};
    m_whiteColor = {0x4B,0x99,0xC5,0xFF};
    m_blackColor = {0x00,0x6A,0xA6,0xFF};
}

void Board::update(int ticks) {}

void Board::draw(SDL_Renderer *renderer) {
    int white=1;
    for(int y=0; y<8; y++) {
        for(int x=0; x<8; x++) {
            int xx=m_rect.x+m_rect.w*x;
            int yy=m_rect.y+m_rect.h*y;
            SDL_Rect r = {xx,yy,m_rect.w,m_rect.h};
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
}
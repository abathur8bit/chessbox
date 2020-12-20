//
// Created by patte on 12/20/2020.
//

#include "TextArea.h"

TextArea::TextArea(string id,int x,int y,int w,int h) : Component(id,x,y,w,h),m_text() {
    m_fontTexture = nullptr;
    m_font = TTF_OpenFont("assets/fonts/Inconsolata-Medium.ttf", 16);
//        m_font = TTF_OpenFont("assets/fonts/FiraSans-Book.otf", 16);
    if (!m_font) {
        printf("TTF_OpenFont: %s\n", TTF_GetError());
        // handle error
    }
}

void TextArea::setText(const char *s) {
    m_text = s;
}

void TextArea::appendText(const char *s) {
    m_text += s;
}

void TextArea::draw(SDL_Renderer *renderer) {
    if (!m_fontTexture) {
        SDL_Color textColor = { 0, 0, 0 };
        SDL_Surface* surface = TTF_RenderText_Blended(m_font, m_text.c_str(), textColor);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        m_fontTexture = texture;
        SDL_QueryTexture(m_fontTexture, NULL, NULL, &m_fontRect.w, &m_fontRect.h);
        m_fontRect.x = m_rect.x + m_rect.w / 2 - m_fontRect.w / 2;
        m_fontRect.y = m_rect.y + m_rect.h / 2 - m_fontRect.h / 2;
        SDL_FreeSurface(surface);
    }
    //find text size, and center text on button
    SDL_RenderCopy(renderer, m_fontTexture, NULL, &m_fontRect);

}

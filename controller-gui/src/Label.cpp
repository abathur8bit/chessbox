//
// Created by patte on 12/20/2020.
//

#include "Label.h"

Label::Label(string id, int x, int y, int w, int h) : Component(id, x, y, w, h), m_text() {
    m_fontTexture = nullptr;
    m_color = {0,0,0};
    m_font = TTF_OpenFont("assets/fonts/Inconsolata-Medium.ttf", 20);
//        m_font = TTF_OpenFont("assets/fonts/FiraSans-Book.otf", 16);
    if (!m_font) {
        printf("TTF_OpenFont: %s\n", TTF_GetError());
        // handle error
    }
}

void Label::setText(const char *s) {
    m_text = s;
    invalidateTexture();
}

void Label::appendText(const char *s) {
    m_text += s;
    invalidateTexture();
}

void Label::invalidateTexture() {
    if(m_fontTexture) {
        SDL_DestroyTexture(m_fontTexture);
        m_fontTexture= nullptr;
    }
}

void Label::draw(SDL_Renderer *renderer) {
    if (!m_fontTexture) {
        SDL_Surface* surface = TTF_RenderText_Blended(m_font, m_text.c_str(), m_color);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        m_fontTexture = texture;
        SDL_QueryTexture(m_fontTexture, NULL, NULL, &m_fontRect.w, &m_fontRect.h);
        m_fontRect.x = m_rect.x;
        m_fontRect.y = m_rect.y;
        SDL_FreeSurface(surface);
    }
    //find text size, and center text on button
    SDL_RenderCopy(renderer, m_fontTexture, NULL, &m_fontRect);

}
void Label::setColor(SDL_Color &c) {
    m_color = c;
}
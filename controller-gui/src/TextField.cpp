//
// Created by patte on 12/26/2020.
//

#include "TextField.h"

TextField::TextField(string id, int x, int y, int w, int h)
    : Component(id,x,y,w,h),
    m_text(),
    m_fontTexture(nullptr),
    m_font(nullptr),
    m_fontRect()
{
    m_backgroundColor={255,255,255};
    m_font = TTF_OpenFont("assets/fonts/Inconsolata-Medium.ttf", 16);
    if (!m_font) {
        printf("TTF_OpenFont: %s\n", TTF_GetError());
    }
}

void TextField::setText(const char *s) {
    m_text = s;
    invalidateTexture();
}

void TextField::invalidateTexture() {
    if(m_fontTexture) {
        SDL_DestroyTexture(m_fontTexture);
        m_fontTexture= nullptr;
    }
}
void TextField::draw(SDL_Renderer *renderer) {
    SDL_Color outline={m_backgroundColor.r,m_backgroundColor.g,m_backgroundColor.b};
    outline.r/=2;
    outline.g/=2;
    outline.b/=2;
    SDL_SetRenderDrawColor(renderer, m_backgroundColor.r, m_backgroundColor.g, m_backgroundColor.b, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer, &m_rect);
    SDL_SetRenderDrawColor(renderer, outline.r, outline.g, outline.b, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawRect(renderer, &m_rect);

    //message
    if (!m_fontTexture) {
        //find text size, and center text on button
        SDL_Color textColor = { 0, 0, 0 };
        SDL_Surface* surface = TTF_RenderText_Blended(m_font, m_text.c_str(), textColor);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        m_fontTexture = texture;
        SDL_QueryTexture(m_fontTexture, NULL, NULL, &m_fontRect.w, &m_fontRect.h);
        m_fontRect.x = m_rect.x+5;
        m_fontRect.y = m_rect.y + m_rect.h / 2 - m_fontRect.h / 2;
        SDL_FreeSurface(surface);
    }
    SDL_RenderCopy(renderer, m_fontTexture, NULL, &m_fontRect);

}

void TextField::update(long ticks) {

}
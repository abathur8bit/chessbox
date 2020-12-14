//
// Created by patte on 12/14/2020.
//

#include "Button.h"

Button::Button(string id,int x,int y,int w,int h) {
    m_fontTexture = nullptr;
//    m_listener = nullptr;

    m_sourceRect = {x,y,w,h};
    m_state = 0;
    m_text=id;
    if(!m_text.empty()) {
        m_font=TTF_OpenFont("assets/fonts/FiraSans-Book.otf",16);
//        m_font=TTF_OpenFont("assets/fonts/FreeSans.ttf",16);
        if(!m_font) {
            printf("TTF_OpenFont: %s\n", TTF_GetError());
            // handle error
        }
    }
}

void Button::setListener(void(*f)()) {
    printf("listener being called\n");
    f();
}

Button::~Button() {
//    if(m_font) {
//        TTF_CloseFont(m_font);
//        m_font=nullptr;
//    }
}

void Button::draw(SDL_Renderer* renderer) {
    switch(m_state) {
        case BUTTON_STATE_NORMAL:
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
            break;
        case BUTTON_STATE_MOUSE_OVER:
            SDL_SetRenderDrawColor(renderer, 200, 0, 0, SDL_ALPHA_OPAQUE);
            break;
        case BUTTON_STATE_MOUSE_DOWN:
            SDL_SetRenderDrawColor(renderer, 255, 128, 0, SDL_ALPHA_OPAQUE);
//            if(m_listener)
//                m_listener->handle(nullptr);
            break;
    }
    SDL_RenderFillRect(renderer, &m_sourceRect);
    SDL_SetRenderDrawColor(renderer, 100, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawRect(renderer, &m_sourceRect);

    if(!m_fontTexture) {
        SDL_Color textColor = {0, 0, 0};;
        SDL_Surface *surface = TTF_RenderText_Blended(m_font, m_text.c_str(), textColor);
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
        m_fontTexture = texture;
    }
    SDL_Rect r={m_sourceRect.x+5,m_sourceRect.y+5,0,0};
    SDL_QueryTexture(m_fontTexture, NULL, NULL, &r.w, &r.h);
////    r.h = 16;
//
    SDL_RenderCopy(renderer, m_fontTexture, NULL, &r);
//
//    SDL_DestroyTexture(m_fontTexture);
//    SDL_FreeSurface(surface);
}

bool Button::mouseEvent(SDL_Event event,void(*f)()) {
//    printf("x=%d y=%d button=%d\n",event.button.x,event.button.y,event.button.button);
    if(event.button.x>=m_sourceRect.x && event.button.x<m_sourceRect.x+m_sourceRect.w && event.button.y>=m_sourceRect.y && event.button.y<m_sourceRect.y+m_sourceRect.h) {
        switch(event.type) {
            case  SDL_MOUSEMOTION:
                m_state=BUTTON_STATE_MOUSE_OVER;
                break;
            case SDL_MOUSEBUTTONDOWN:
                m_state=BUTTON_STATE_MOUSE_DOWN;
                break;
            case SDL_MOUSEBUTTONUP:
                m_state=BUTTON_STATE_MOUSE_OVER;
                f();
                break;
        }
    } else {
        m_state=BUTTON_STATE_NORMAL;
    }

    return false;
}

AnimButton::AnimButton(string id,AnimSprite sprite) : Button(id,sprite.destRect().x-10,sprite.destRect().y-10,sprite.widthDest()+20,sprite.heightDest()+20) {
    m_sprite = sprite;
}
void AnimButton::update(int ticks) {
    m_sprite.update(ticks);
}
void AnimButton::draw(SDL_Renderer* renderer) {
    Button::draw(renderer);
    m_sprite.draw(renderer);
}

//
// Created by patte on 12/14/2020.
//

#include "Button.h"

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
            break;
    }
    SDL_RenderFillRect(renderer, &m_sourceRect);
    SDL_SetRenderDrawColor(renderer, 100, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawRect(renderer, &m_sourceRect);
}

bool Button::mouseEvent(SDL_Event event) {
    printf("x=%d y=%d button=%d\n",event.button.x,event.button.y,event.button.button);
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

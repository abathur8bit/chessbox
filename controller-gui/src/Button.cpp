/* *****************************************************************************
 * Created by Lee Patterson 12/19/2020
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

#include "Button.h"
#include "FontManager.h"

Button::Button(string id,int x,int y,int w,int h) : Component(id,x,y,w,h){
    m_checked = false;
    m_normalColor    = {0x52,0xE5,0x00};
    m_mouseOverColor = {0xBF,0xE5,0x00};
    m_mouseDownColor = {0x35,0x9E,0x00};
    m_checkedColor   = {0xE3,0xE7,0x00};
    m_state = 0;
}

Button::~Button() {
}

void Button::setChecked(bool state) {
    m_checked = state;
}

void Button::draw(SDL_Renderer* renderer) {
    SDL_Color buttonColor = m_normalColor;
    if(m_checked)
        buttonColor=m_checkedColor;
    switch (m_state) {
        case BUTTON_STATE_MOUSE_DOWN:
            buttonColor=m_mouseDownColor;
            break;
        case BUTTON_STATE_MOUSE_OVER:
            buttonColor.r+=15;
            buttonColor.g+=15;
            buttonColor.b+=150;
            break;
    }
    SDL_SetRenderDrawColor(renderer, buttonColor.r, buttonColor.g, buttonColor.b, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer, &m_rect);
    SDL_SetRenderDrawColor(renderer, 100, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawRect(renderer, &m_rect);

    //draw line through middle of button
//    SDL_Rect center={m_rectSquare.x,m_rectSquare.y+m_rectSquare.h/2,m_rectSquare.w,m_rectSquare.h/2};
//    SDL_RenderDrawRect(m_renderer, &center);
}

Component* Button::mouseEvent(SDL_Event* event) {
    if(event->button.x >= m_rect.x && event->button.x < m_rect.x + m_rect.w && event->button.y >= m_rect.y && event->button.y < m_rect.y + m_rect.h) {
        switch(event->type) {
            case  SDL_MOUSEMOTION:
                m_state=BUTTON_STATE_MOUSE_OVER;
                return this;
                break;
            case SDL_MOUSEBUTTONDOWN:
                m_state=BUTTON_STATE_MOUSE_DOWN;
                return this;
                break;
            case SDL_MOUSEBUTTONUP:
                m_state=BUTTON_STATE_MOUSE_OVER;
                //f(this);
                return this;
                break;
        }
    } else {
        m_state=BUTTON_STATE_NORMAL;
    }

    return nullptr;
}

AnimButton::AnimButton(const char* id,SDL_Renderer* renderer,const char* filename,int frameCount,int x,int y) : Button(id,x,y,0,0),m_sprite(id),m_delay(0) {
    const int border=5;
    m_sprite.load(renderer, filename, frameCount, x, y);
    m_sprite.setPos(border, border);
    m_rect.w = m_sprite.destRect()->w + border + border;   //border*2
    m_rect.h = m_sprite.destRect()->h + border + border;
}
void AnimButton::update(long ticks) {
    m_sprite.update(ticks);
}
void AnimButton::draw(SDL_Renderer* renderer) {
    Button::draw(renderer);
    SDL_Rect r;
    copyRect(&r,m_sprite.destRect());
    m_sprite.destRect()->x += m_rect.x;
    m_sprite.destRect()->y += m_rect.y;
    m_sprite.draw(renderer);
    copyRect(m_sprite.destRect(),&r);
}

TextButton::TextButton(string id, string text, int x, int y, int w, int h)
    : Button(id, x, y, w, h),
      m_text(text),
      m_fontTexture(nullptr),
      m_font(nullptr)
{
    if (!m_text.empty()) {
        m_font = FontManager::instance()->font("normal");
    }
}

void TextButton::setText(const char *s) {
    m_text = s;
    invalidateTexture();
}

void TextButton::invalidateTexture() {
    if(m_fontTexture) {
        SDL_DestroyTexture(m_fontTexture);  //todo when this happens, the chess board blacks out, need to find out why
        m_fontTexture= nullptr;
    }
}


void TextButton::draw(SDL_Renderer* renderer) {
    if(m_text.empty())
        return;

    Button::draw(renderer);
    if (!m_fontTexture) {
        //find text size, and center text on button
        SDL_Color textColor = { 0, 0, 0 };
        SDL_Surface* surface = TTF_RenderText_Blended(m_font, m_text.c_str(), textColor);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        m_fontTexture = texture;
        SDL_QueryTexture(m_fontTexture, NULL, NULL, &m_fontRect.w, &m_fontRect.h);
        m_fontRect.x = m_rect.x + m_rect.w / 2 - m_fontRect.w / 2;
        m_fontRect.y = m_rect.y + m_rect.h / 2 - m_fontRect.h / 2;
        SDL_FreeSurface(surface);
    }
    SDL_RenderCopy(renderer, m_fontTexture, NULL, &m_fontRect);
}
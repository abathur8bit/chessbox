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

#ifndef CONTROLLER_GUI_BUTTON_H
#define CONTROLLER_GUI_BUTTON_H

#include <string>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "Component.h"
#include "Sprite.h"

using namespace std;

#define BUTTON_STATE_NORMAL 0
#define BUTTON_STATE_MOUSE_OVER 1
#define BUTTON_STATE_MOUSE_DOWN 2
#define BUTTON_STATE_MOUSE_UP   4
#define BUTTON_STATE_CHECKED    8

class Button : public Component {
public:
    /** \brief Button should be a min of 60x60 to make it easy to tap on eboard. */
    Button(string id,int x,int y,int w,int h);
    ~Button();
    virtual void update(long ticks){}
    virtual void draw(SDL_Renderer* renderer);
    virtual Component* mouseEvent(SDL_Event* event);
//    virtual bool mouseEvent(SDL_Event* event,void(*f)(Button* b));
    void setChecked(bool state);
    bool isChecked() {return m_checked;}
protected:
    int m_state;
    bool m_checked;
    SDL_Color m_normalColor;
    SDL_Color m_mouseOverColor;
    SDL_Color m_mouseDownColor;
    SDL_Color m_checkedColor;
};

class AnimButton : public Button {
public:
    AnimButton(const char* id,SDL_Renderer* renderer,const char* filename,int frameCount,int x,int y);
    virtual void update(long ticks);
    virtual void draw(SDL_Renderer* renderer);
protected:
    Sprite m_sprite;
    Uint32 m_delay;
};

class TextButton : public Button {
public:
    TextButton(string id, string text, int x, int y, int w, int h);
    void setText(const char* s);
    virtual const char* text() { return m_text.c_str(); }
    virtual void draw(SDL_Renderer* renderer);
protected:
    void invalidateTexture();
    string m_text;
    SDL_Texture* m_fontTexture;
    TTF_Font* m_font;
    SDL_Rect m_fontRect;
};

#endif //CONTROLLER_GUI_BUTTON_H

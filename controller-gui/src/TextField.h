//
// Created by patte on 12/26/2020.
//

#ifndef CONTROLLER_GUI_TEXTFIELD_H
#define CONTROLLER_GUI_TEXTFIELD_H

#include <SDL.h>
#include <SDL_ttf.h>

#include <string>

#include "Component.h"

//SDL_bool SDL_HasClipboardText(void)
//char* SDL_GetClipboardText(void)
//int SDL_SetClipboardText(const char* text)
class TextField : public Component {
public:
    TextField(string id,int x,int y,int w,int h);
    virtual void draw(SDL_Renderer* renderer);
    virtual void update(long ticks);
    void setText(const char* s);
    const char* text() {return m_text.c_str();}
    void invalidateTexture();

protected:
    string m_text;
    SDL_Texture* m_fontTexture;
    TTF_Font* m_font;
    SDL_Rect m_fontRect;
};


#endif //CONTROLLER_GUI_TEXTFIELD_H

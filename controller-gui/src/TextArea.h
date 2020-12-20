//
// Created by patte on 12/20/2020.
//

#ifndef CONTROLLER_GUI_TEXTAREA_H
#define CONTROLLER_GUI_TEXTAREA_H

#include <SDL.h>
#include <SDL_ttf.h>

#include "Component.h"

class TextArea : public Component {
public:
    TextArea(string id,int x,int y,int w,int h);
    virtual void draw(SDL_Renderer* r);
    void setText(const char* s);
    void appendText(const char* s);
protected:
    string m_text;
    SDL_Texture* m_fontTexture;
    TTF_Font* m_font;
    SDL_Rect m_fontRect;
};


#endif //CONTROLLER_GUI_TEXTAREA_H

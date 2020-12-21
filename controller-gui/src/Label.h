//
// Created by patte on 12/20/2020.
//

#ifndef CONTROLLER_GUI_LABEL_H
#define CONTROLLER_GUI_LABEL_H

#include <SDL.h>
#include <SDL_ttf.h>

#include "Component.h"

class Label : public Component {
public:
    Label(string id, int x, int y, int w, int h);
    virtual void draw(SDL_Renderer* r);
    void setText(const char* s);
    void appendText(const char* s);
    void invalidateTexture();
    void setColor(SDL_Color& c);
protected:
    string m_text;
    SDL_Texture* m_fontTexture;
    TTF_Font* m_font;
    SDL_Rect m_fontRect;
    SDL_Color m_color;
};


#endif //CONTROLLER_GUI_LABEL_H

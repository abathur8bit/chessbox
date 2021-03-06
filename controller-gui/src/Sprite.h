//
// Created by patte on 12/13/2020.
//

#ifndef CONTROLLER_GUI_SPRITE_H
#define CONTROLLER_GUI_SPRITE_H

#include <string>
#include <SDL.h>
#include <SDL_image.h>

#include "Component.h"

using namespace std;

#define SCALE 1

class Sprite : public Component
{
public:
    Sprite(const char* id);
    SDL_Texture * load(SDL_Renderer* renderer, const char* filename, int x, int y);
    SDL_Texture* load(SDL_Renderer* renderer, const char* filename, int frameCount, int x, int y);
    void draw(SDL_Renderer* renderer,SDL_Rect* dest);
    virtual void draw(SDL_Renderer* renderer);
    virtual void update(long ticks);
    void setPos(int x, int y);
    SDL_Rect* destRect() {return &m_rect;}
    void setFrame(int frame);
    void setDelay(Uint32 delayMilliSeconds);
    int widthDest() { return m_rect.w; }
    int heightDest() { return m_rect.h; }
    int widthSource() { return m_sourceRect.w; }
    int heightSource() { return m_sourceRect.h; }
    const char* name() {return m_name.c_str();}
protected:
    SDL_Texture* m_texture;
    int m_frameCount;
    int m_currentFrame;
    Uint32 m_delay;
    SDL_Rect m_sourceRect;
    string m_name;
};


#endif //CONTROLLER_GUI_SPRITE_H

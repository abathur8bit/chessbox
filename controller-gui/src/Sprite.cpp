//
// Created by patte on 12/13/2020.
//

#include "Sprite.h"

Sprite::Sprite(const char *id) : Component(id,0,0,0,0) {
}
SDL_Texture* Sprite::load(SDL_Renderer* renderer,const char* filename,int x,int y) {
    return load(renderer,filename,1,x,y);
}
SDL_Texture* Sprite::load(SDL_Renderer* renderer, const char* filename, int frameCount, int x, int y)
{
    m_name = filename;
    m_texture = IMG_LoadTexture(renderer, filename);
    m_delay = 100;

    m_currentFrame = 0;
    m_sourceRect.x = 0;
    m_sourceRect.y = 0;
    SDL_QueryTexture(m_texture, NULL, NULL, &m_sourceRect.w, &m_sourceRect.h);
    m_sourceRect.w /= frameCount;   //each frame of the animation
    m_frameCount = frameCount;
    m_rect.x = x-m_sourceRect.w/2;
    m_rect.y = y-m_sourceRect.h/2;
    m_rect.w = m_sourceRect.w * SCALE;
    m_rect.h = m_sourceRect.h * SCALE;
    return m_texture;
}
void Sprite::draw(SDL_Renderer* renderer,SDL_Rect* dest) {
    SDL_RenderCopyEx(renderer, m_texture, &m_sourceRect, dest, 0, 0, SDL_FLIP_NONE);
}
void Sprite::draw(SDL_Renderer* renderer) {
    SDL_RenderCopyEx(renderer, m_texture, &m_sourceRect, &m_rect, 0, 0, SDL_FLIP_NONE);
}
void Sprite::update(long ticks)
{
    m_sourceRect.x = m_sourceRect.w * int(((ticks / m_delay) % m_frameCount));
}
void Sprite::setPos(int x, int y)
{
    m_rect.x = x;
    m_rect.y = y;
}
void Sprite::setFrame(int frame)
{
    m_currentFrame = frame;
}
void Sprite::setDelay(Uint32 delayMilliSeconds)
{
    m_delay = delayMilliSeconds;
}

//
// Created by patte on 12/13/2020.
//

#include "AnimSprite.h"

SDL_Texture* AnimSprite::load(SDL_Renderer* renderer, string filename, int frameCount, int x, int y)
{
    m_texture = IMG_LoadTexture(renderer, filename.c_str());
    m_delay = 100;

    m_currentFrame = 0;
    m_sourceRect.x = 0;
    m_sourceRect.y = 0;
    SDL_QueryTexture(m_texture, NULL, NULL, &m_sourceRect.w, &m_sourceRect.h);
    m_sourceRect.w /= frameCount;   //each frame of the animation
    m_frameCount = frameCount;
    m_destinationRect.x = x;
    m_destinationRect.y = y;
    m_destinationRect.w = m_sourceRect.w * SCALE;
    m_destinationRect.h = m_sourceRect.h * SCALE;
    return m_texture;
}
void AnimSprite::draw(SDL_Renderer* renderer)
{
    SDL_RenderCopyEx(renderer, m_texture, &m_sourceRect, &m_destinationRect, 0, 0, SDL_FLIP_NONE);
}
void AnimSprite::update(int ticks)
{
    m_sourceRect.x = m_sourceRect.w * int(((ticks / m_delay) % m_frameCount));

}
void AnimSprite::setPos(int x, int y)
{
    m_destinationRect.x = x;
    m_destinationRect.y = y;
}
void AnimSprite::setFrame(int frame)
{
    m_currentFrame = frame;
}
void AnimSprite::setDelay(Uint32 delayMilliSeconds)
{
    m_delay = delayMilliSeconds;
}

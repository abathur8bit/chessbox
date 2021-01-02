//
// Created by patte on 12/27/2020.
//

#include "Window.h"

Window::Window(const char *id, int x, int y, int w, int h)
    : Component(id,x,y,w,h),
      m_components(),
      m_selection(0),
      m_showing(false)
{
    m_backgroundColor={191,37,0};
}
void Window::addComponent(Component *c) {
    m_components.push_back(c);
}
void Window::addButton(Button *c) {
    m_buttons.push_back(c);
}
void Window::draw(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, m_backgroundColor.r, m_backgroundColor.g, m_backgroundColor.b, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    for (list<Component *>::iterator it = m_components.begin(); it != m_components.end(); it++) {
        (*it)->draw(renderer);
    }
}
void Window::update(long ticks) {
    for (list<Component *>::iterator it = m_components.begin(); it != m_components.end(); it++) {
        (*it)->update(ticks);
    }
}
void Window::show(SDL_Renderer* renderer) {
    m_showing=true;
    while (m_showing)
    {
        SDL_Event event;
        if (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEBUTTONUP:
                case SDL_MOUSEMOTION:
                    processMouseEvent(&event);
                    break;
                case SDL_QUIT:
                    m_showing=false;
                    break;
                case SDL_KEYDOWN:
                    if(SDL_SCANCODE_ESCAPE==event.key.keysym.scancode)
                        m_showing=false;
                    break;
                default:
                    break;
            }
        }
        draw(renderer);
        SDL_RenderPresent(renderer);
        update(SDL_GetTicks());
    }
}
void Window::processMouseEvent(SDL_Event *event) {
    for (list<Button *>::iterator it = m_buttons.begin(); it != m_buttons.end(); it++) {
        Button* result=static_cast<Button*>((*it)->mouseEvent(event));
        if(result && SDL_MOUSEBUTTONUP==event->type)
            processButtonClicked(result);
    }
}

Button* Window::findButton(const char *name) {
    for (list<Button *>::iterator it = m_buttons.begin(); it != m_buttons.end(); it++) {
        if(!strcmp((*it)->id(),name))
            return *it;
    }
    return nullptr;
}

Component* Window::findComponent(const char *name) {
    for (list<Component *>::iterator it = m_components.begin(); it != m_components.end(); it++) {
        if(!strcmp((*it)->id(),name))
            return *it;
    }
    return nullptr;
}
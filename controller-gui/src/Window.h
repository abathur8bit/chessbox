//
// Created by patte on 12/27/2020.
//

#ifndef CONTROLLER_GUI_WINDOW_H
#define CONTROLLER_GUI_WINDOW_H

#include <string>
#include <list>

#include "Component.h"
#include "Button.h"

using namespace std;

class Window : public Component {
public:
    Window(const char* id,int x,int y,int w,int h);
    virtual void draw(SDL_Renderer* renderer);
    virtual void update(long ticks);
    virtual void processButtonClicked(Button* c) {}      ///< Default behaviour is to do nothing
    void addComponent(Component* c);
    void addButton(Button* b);
    void show(SDL_Renderer* renderer);
    void processMouseEvent(SDL_Event* event);
protected:
    SDL_Color m_backgroundColor;
    list<Component*> m_components;
    list<Button*> m_buttons;
    int m_selection;
    bool m_showing;
};


#endif //CONTROLLER_GUI_WINDOW_H

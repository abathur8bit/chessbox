//
// Created by patte on 12/19/2020.
//

#ifndef CONTROLLER_GUI_UIGROUP_H
#define CONTROLLER_GUI_UIGROUP_H

#include <list>

#include "Component.h"

class UIGroup : public Component {
public:
    UIGroup(const char* id,int x,int y,int w,int h);
    const char* id() { return m_id.c_str(); }
    virtual void add(Component* c);
    virtual void remove(Component* c);
    virtual void update(long ticks);
    virtual void draw(SDL_Renderer* r);
    list<Component*>* components() {return &m_components;}
    list<Component *>::iterator begin();
    list<Component *>::iterator end();
    Component* mouseEvent(SDL_Event* event);
protected:
    list<Component*> m_components;
};


#endif //CONTROLLER_GUI_UIGROUP_H

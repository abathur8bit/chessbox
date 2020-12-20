//
// Created by patte on 12/19/2020.
//

#include "UIGroup.h"

UIGroup::UIGroup(const char *id, int x, int y, int w, int h) : Component(id,x,y,w,h) ,m_components() {

}

void UIGroup::add(Component* c) {
    m_components.push_back(c);
    
}

void UIGroup::remove(Component *c) {
    m_components.remove(c);
}

list<Component *>::iterator UIGroup::begin() {
    return m_components.begin();
}

list<Component *>::iterator UIGroup::end(){
    return m_components.end();
}

void UIGroup::update(long ticks) {
    for (list<Component *>::iterator it = m_components.begin(); it != m_components.end(); it++) {
        (*it)->update(ticks);
    }
}

void UIGroup::draw(SDL_Renderer *renderer) {
    SDL_Rect r;
    for(list<Component*>::iterator it=m_components.begin(); it != m_components.end(); it++) {
        //adjust the offset of the component to be relative to this group
        Component* c = *it;
        copyRect(&r,c->rect()); //save
        c->rect()->x+=m_rect.x;
        c->rect()->y+=m_rect.y;
        c->draw(renderer);
        copyRect(c->rect(),&r); //restore
    }
}

Component* UIGroup::mouseEvent(SDL_Event* event) {
    SDL_Rect r;
    for(list<Component*>::iterator it=m_components.begin(); it != m_components.end(); ++it) {
        Component* c = *it;
        copyRect(&r,c->rect()); //save
        c->rect()->x+=m_rect.x; //adjust offset
        c->rect()->y+=m_rect.y;
        Component* result = (*it)->mouseEvent(event);
        copyRect(c->rect(),&r); //restore
        if(result)
            return result;
//        (*it)->mouseEvent(event,[](Component* b){printf("component %s was clicked size=%dx%d\n",b->id(),b->rect()->w,b->rect()->h);});
    }
    return nullptr;
}

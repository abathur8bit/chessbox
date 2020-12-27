/* *****************************************************************************
 * Created by Lee Patterson 12/19/2020
 *
 * Copyright 2019 Lee Patterson <https://github.com/abathur8bit>
 *
 * You may use and modify at will. Please credit me in the source.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * ******************************************************************************/

#ifndef CONTROLLER_GUI_COMPONENT_H
#define CONTROLLER_GUI_COMPONENT_H

#include <string>

#include <SDL.h>

using namespace std;

/** \brief Base class for GUI widgets. */
class Component {
public:
    Component(string id,int x,int y,int w,int h);
    ~Component() {}
    virtual void draw(SDL_Renderer* renderer)=0;
    virtual void update(long ticks){}
    virtual Component * mouseEvent(SDL_Event* event);
//    virtual bool mouseEvent(SDL_Event* event,void(*f)(Component* b));
    virtual const char* id() { return m_id.c_str(); }
    virtual SDL_Rect* rect() {return &m_rect;}
    void copyRect(SDL_Rect* dest,SDL_Rect* src);
    SDL_Color backgroundColor();
protected:
    SDL_Rect m_rect;
    string m_id;
    SDL_Color m_backgroundColor;
};


#endif //CONTROLLER_GUI_COMPONENT_H

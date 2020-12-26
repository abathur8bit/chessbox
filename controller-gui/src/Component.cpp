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

#include "Component.h"

Component::Component(string id, int x, int y, int w, int h) {
    m_rect={x, y, w, h};
    m_id=id;
    m_backgroundColor={191,37,0};
}

void Component::copyRect(SDL_Rect* dest,SDL_Rect* src) {
    dest->x=src->x;
    dest->y=src->y;
    dest->w=src->w;
    dest->h=src->h;
}

Component* Component::mouseEvent(SDL_Event* event) {
    return nullptr;
}
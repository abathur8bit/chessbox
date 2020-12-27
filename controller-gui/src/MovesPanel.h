/* *****************************************************************************
 * Created by Lee Patterson 12/21/2020
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

#ifndef CONTROLLER_GUI_MOVESPANEL_H
#define CONTROLLER_GUI_MOVESPANEL_H

#include <list>

#include "thc.h"
#include "Component.h"
#include "Label.h"
#include "Board.h"

#define MOVE_WHITE 0
#define MOVE_BLACK 1

#define NUM_LINES 16

class MovesPanel : public Component {
public:
    MovesPanel(const char* id,int x,int y,int w,int h,BoardRules* rules);
    virtual void draw(SDL_Renderer* r);
    virtual void update(long ticks);

    void add(thc::Move mv);
    void add(const char* s);
    void showMove(int moveNum,int side);
    void scroll(int n);
    void clear();
    const char* text(int i) {return m_moveLabels[i]->getText();}
    void init();
protected:
    Label* m_moveLabels[NUM_LINES+1];
    int m_currentLine;
    BoardRules* m_rules;
};


#endif //CONTROLLER_GUI_MOVESPANEL_H

//
// Created by patte on 12/20/2020.
//

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
    void showMove(int moveNum,int side);

protected:
    Label* m_moveLabels[NUM_LINES];
    BoardRules* m_rules;
};


#endif //CONTROLLER_GUI_MOVESPANEL_H

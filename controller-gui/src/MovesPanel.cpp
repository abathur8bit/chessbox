//
// Created by patte on 12/20/2020.
//

#include "MovesPanel.h"

MovesPanel::MovesPanel(const char *id, int x, int y, int w, int h,BoardRules* rules) : Component(id,x,y,w,h),m_moveLabels(),m_rules(rules) {
    int ww=180;
    int hh=20;
    int xx=480-200+5;
    int yy=481;
    char buffer[255];
    for(int i=0; i<NUM_LINES; i++) {
        snprintf(buffer,sizeof(buffer),"moveline%02d",i);
        m_moveLabels[i] = new Label(buffer,xx,yy,ww,12,16);
//        m_moveLabels[i]->setText(buffer);
        yy+=hh;
    }
}

void MovesPanel::add(thc::Move mv) {
    m_rules->PlayMove(mv);
    char buffer[80];
    string output;
    int lineNum = 0;
    int index = 1;
    while(index<m_rules->historyIndex() && lineNum<NUM_LINES) {
        snprintf(buffer,sizeof(buffer),"%d.",(lineNum+1));
        output += buffer;
        snprintf(buffer,sizeof(buffer),"%-5s",m_rules->historyAt(index).TerseOut().c_str());
        output += buffer;
        index++;
        if(index<m_rules->historyIndex()) {
            //add second half of move
            output += "  ";
            snprintf(buffer,sizeof(buffer),"%-5s",m_rules->historyAt(index).TerseOut().c_str());
            output += buffer;
            index++;
        }
        m_moveLabels[lineNum]->setText(output.c_str());
        lineNum++;
        output.clear();
    }
}

void MovesPanel::draw(SDL_Renderer *r) {
    for(int i=0; i<NUM_LINES; i++) {
        m_moveLabels[i]->draw(r);
    }
}

void MovesPanel::update(long ticks) {
    for(int i=0; i<NUM_LINES; i++) {
        m_moveLabels[i]->update(ticks);
    }
}

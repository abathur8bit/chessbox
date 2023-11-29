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

#include "MovesPanel.h"

MovesPanel::MovesPanel(const char *id, int x, int y, int w, int h,BoardRules* rules)
    : Component(id,x,y,w,h),m_moveLabels(),m_currentLine(0),m_rules(rules)
{
}

void MovesPanel::init() {
    int ww=m_rect.w;
    int hh=20;
    int xx=m_rect.x;
    int yy=m_rect.y;
    char buffer[255];
    int lineNum=0;
    const char* lines[] = {
            "1. d4 d5",
            "2. c4 dxc4",
            "  (2... e5",
            "   3. Nc3 Nc6",
            "     (3... Bb4",
            "      4. O-O-O  O-O-O)",
            "   4. Nf3 e4)",
            "3. e3 b5",
            "4. a4 a6",
            "5. axb5",

    };
    for(int i=0; i<NUM_LINES+1; i++) {
        snprintf(buffer,sizeof(buffer),"moveline%02d",i);
        m_moveLabels[i] = new Label(buffer,xx,yy,ww,12);
//        if(lineNum<9) {
//            snprintf(buffer, sizeof(buffer), "%s", lines[lineNum]);
//            m_moveLabels[i]->setText(buffer);
//            lineNum++;
//        }
        yy+=hh;
    }
}
void MovesPanel::add(const char* s) {
    if(m_currentLine == NUM_LINES) {
        scroll(1);
        m_moveLabels[m_currentLine-1]->setText(s);
    } else {
        m_moveLabels[m_currentLine]->setText(s);
        m_currentLine++;
    }
}

void MovesPanel::add(thc::Move mv) {
    string san = mv.NaturalOut(m_rules);
    char buffer[80];
    int historyIndex = m_rules->historyIndex();
    int moveNum = (historyIndex-1)/2+1;
    if(m_rules->WhiteToPlay()) {
        //white moving, display whites move
        snprintf(buffer,sizeof(buffer)," %d. %s",moveNum,san.c_str());
        add(buffer);
    } else {
        //black moving, display blacks move
//        snprintf(buffer,sizeof(buffer),"%d. %s %s",moveNum,m_rules->historySanAt(historyIndex-1).c_str(),san.c_str());
        //grab the previous text and append blacks move as we can format it better
        snprintf(buffer,sizeof(buffer)," %-10s   %s",m_moveLabels[m_currentLine-1]->getText(),san.c_str());
        m_moveLabels[m_currentLine-1]->setText(buffer);
    }
    m_rules->PlayMove(mv);
}

void MovesPanel::draw(SDL_Renderer *renderer) {
    int x=m_rect.x;
    int y=m_rect.y;
    int w=m_rect.w;
    int h=m_rect.h/NUM_LINES;
    SDL_Rect bg={x,y,w,h};
    SDL_Color dark={0,153,156};
    SDL_Color light={0,207,210};
    for(int i=0; i<NUM_LINES; i++) {
        if(i%2==0)
            SDL_SetRenderDrawColor(renderer, dark.r, dark.g, dark.b, SDL_ALPHA_OPAQUE);
        else
            SDL_SetRenderDrawColor(renderer, light.r, light.g, light.b, SDL_ALPHA_OPAQUE);
        SDL_RenderFillRect(renderer, &bg);
        m_moveLabels[i]->draw(renderer);
        bg.y+=h;
    }
}

void MovesPanel::update(long ticks) {
    for(int i=0; i<NUM_LINES; i++) {
        m_moveLabels[i]->update(ticks);
    }
}

void MovesPanel::scroll(int n) {
    for(int i=0; i<NUM_LINES-1; i++) {
        printf("%s < %s\n",m_moveLabels[i]->getText(),m_moveLabels[i+1]->getText());
        m_moveLabels[i]->setText(m_moveLabels[i+1]->getText());
    }
}

void MovesPanel::clear() {
    for(int i=0; i<NUM_LINES; i++)
        m_moveLabels[i]->clear();
}
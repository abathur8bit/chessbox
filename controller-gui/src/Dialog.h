//
// Created by patte on 12/26/2020.
//

#ifndef CONTROLLER_GUI_DIALOG_H
#define CONTROLLER_GUI_DIALOG_H

#include "Component.h"
#include "Button.h"

#define DIALOG_TYPE_OK          0
#define DIALOG_TYPE_OK_CANCEL   1
#define DIALOG_TYPE_YES_NO      2
#define DIALOG_TYPE_CANCEL      3

#define DIALOG_SELECTED_NONE    0
#define DIALOG_SELECTED_OKAY    1
#define DIALOG_SELECTED_YES     1
#define DIALOG_SELECTED_CANCEL  2
#define DIALOG_SELECTED_NO      2

class Dialog : public Component {
public:
    Dialog(const char* title,const char* msg,int type);
    virtual void draw(SDL_Renderer* r);
    virtual void update(long ticks);
    virtual int show(SDL_Renderer* renderer);
    virtual int processMouseEvent(SDL_Event* event);

protected:
    SDL_Renderer* m_renderer;
    string m_title;
    string m_message;
    int m_type;
    bool m_invalidated;
    bool m_showing;
    TextButton m_okayButton;
    TextButton m_cancelButton;
    int m_selection;    ///< What the user selected
    SDL_Texture* m_fontTexture;
    TTF_Font* m_font;
    SDL_Rect m_fontRect;
};


#endif //CONTROLLER_GUI_DIALOG_H

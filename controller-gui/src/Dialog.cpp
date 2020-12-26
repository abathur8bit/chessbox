//
// Created by patte on 12/26/2020.
//

#include "Dialog.h"

#define DIALOG_SECTION 60
#define DIALOG_X DIALOG_SECTION
#define DIALOG_Y (DIALOG_SECTION*2)
#define DIALOG_W (DIALOG_SECTION*6)
#define DIALOG_H (DIALOG_SECTION*4)
#define DIALOG_BUTTON_W 100
#define DIALOG_BUTTON_H 40



Dialog::Dialog(const char* title,const char* message,int type)
    : Component(title,60,120,480-120,60*4),
      m_title(title), m_message(message), m_type(type),
      m_invalidated(true), m_showing(true),
      m_okayButton("ok","ok",DIALOG_X+DIALOG_W-DIALOG_BUTTON_W*2-15,DIALOG_Y+DIALOG_H-10-DIALOG_BUTTON_H,DIALOG_BUTTON_W,DIALOG_BUTTON_H),
      m_cancelButton("cancel","cancel",DIALOG_X+DIALOG_W-DIALOG_BUTTON_W-10,DIALOG_Y+DIALOG_H-10-DIALOG_BUTTON_H,DIALOG_BUTTON_W,DIALOG_BUTTON_H),
      m_selection(DIALOG_SELECTED_CANCEL),
      m_font(NULL),m_fontTexture(NULL)
{
    switch(type) {
        case DIALOG_TYPE_OK:
            m_cancelButton.setText("OK");
            break;
        case DIALOG_TYPE_OK_CANCEL:
            m_okayButton.setText("OK");
            m_cancelButton.setText("Cancel");
            break;
        case DIALOG_TYPE_YES_NO:
            m_okayButton.setText("Yes");
            m_cancelButton.setText("No");
            break;
    }
    if (!m_message.empty()) {
        m_font = TTF_OpenFont("assets/fonts/Inconsolata-Medium.ttf", 16);
//        m_font = TTF_OpenFont("assets/fonts/FiraSans-Book.otf", 16);
        if (!m_font) {
            printf("TTF_OpenFont: %s\n", TTF_GetError());
            // handle error
        }
    }
}

void Dialog::draw(SDL_Renderer *renderer) {
    SDL_Color dialogColor={191, 37, 0};
    SDL_Color borderColor={101, 37, 0};

    //dialog
    SDL_SetRenderDrawColor(renderer, dialogColor.r, dialogColor.g, dialogColor.b, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer, &m_rect);
    SDL_SetRenderDrawColor(renderer, borderColor.r, borderColor.g, borderColor.b, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawRect(renderer, &m_rect);

    //message
    if (!m_fontTexture) {
        //find text size, and center text on button
        SDL_Color textColor = { 0, 0, 0 };
        SDL_Surface* surface = TTF_RenderText_Blended(m_font, m_message.c_str(), textColor);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        m_fontTexture = texture;
        SDL_QueryTexture(m_fontTexture, NULL, NULL, &m_fontRect.w, &m_fontRect.h);
        m_fontRect.x = m_rect.x + m_rect.w / 2 - m_fontRect.w / 2;
        m_fontRect.y = m_rect.y + m_rect.h / 2 - m_fontRect.h / 2;
        SDL_FreeSurface(surface);
    }
    SDL_RenderCopy(renderer, m_fontTexture, NULL, &m_fontRect);

    //buttons
    m_okayButton.draw(renderer);
    m_cancelButton.draw(renderer);
}

int Dialog::show(SDL_Renderer* renderer) {
    int selected=DIALOG_SELECTED_NONE;
    while (m_showing)
    {
        SDL_Event event;
        if (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEBUTTONUP:
                case SDL_MOUSEMOTION:
                case SDL_MOUSEWHEEL:
                    selected = processMouseEvent(&event);
                    if(selected) {
                        return selected;
                    }
                    break;
                case SDL_QUIT:
                    m_showing=false;
                    break;
                case SDL_KEYDOWN:
                    m_showing=false;
                    break;
                default:
                    break;
            }
        }

        draw(renderer);
        SDL_RenderPresent(renderer);

        Uint32 ticks = SDL_GetTicks();
        m_okayButton.update(ticks);
        m_cancelButton.update(ticks);
    }

    return m_selection;
}

void Dialog::update(long ticks) {
    Component::update(ticks);
}

int Dialog::processMouseEvent(SDL_Event *event) {
    Component* okay=m_okayButton.mouseEvent(event);
    Component* cancel=m_cancelButton.mouseEvent(event);
    if(event->type==SDL_MOUSEBUTTONUP) {
        if(okay)
            return DIALOG_SELECTED_OKAY;
        if(cancel)
            return DIALOG_SELECTED_CANCEL;
    }
    return DIALOG_SELECTED_NONE;
}

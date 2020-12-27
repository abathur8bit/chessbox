//
// Created by patte on 12/27/2020.
//

#ifndef CONTROLLER_GUI_FONTMANAGER_H
#define CONTROLLER_GUI_FONTMANAGER_H

#include <SDL_ttf.h>

#include <string>
#include <map>

using namespace std;

class FontManager {
private:
    FontManager() {}
    static FontManager* s_pInstance;
    string m_text;
    map<string,TTF_Font*> m_fontMap;
public:
    static FontManager* instance() {
        TTF_Init();
        if(!s_pInstance) {
            s_pInstance = new FontManager();
        }
        return s_pInstance;
    }
    TTF_Font* font(const char* name);
    TTF_Font* add(const char* name,const char* filename,int pointSize=26);      ///< Loads a font.
};


#endif //CONTROLLER_GUI_FONTMANAGER_H

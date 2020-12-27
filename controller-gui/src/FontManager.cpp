//
// Created by patte on 12/27/2020.
//

#include "FontManager.h"

FontManager* FontManager::s_pInstance=nullptr;

/** Filename without the directory, like "Inconsolata-Medium.ttf". */
TTF_Font* FontManager::add(const char *name,const char* filename,int pointSize) {
    string pathname="assets/fonts/";
    pathname+=filename;
    TTF_Font* font=TTF_OpenFont(pathname.c_str(), pointSize);
    if(!font) {
        printf("ERROR: TTF_OpenFont: %s\n", TTF_GetError());
        return nullptr;
    }
    m_fontMap[name]=font;
    return font;
}
TTF_Font* FontManager::font(const char *name) {
    return m_fontMap[name];
}

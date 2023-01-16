//
// Created by patte on 1/15/2023.
//

#ifndef CONTROLLER_GUI_PREFS_H
#define CONTROLLER_GUI_PREFS_H

#include <string>
#include <stdlib.h>
#include <stdio.h>
#include "json.hpp"

using namespace nlohmann;   //json
using namespace std;

class Prefs {
protected:
    string m_filename;
    string m_engine;
    string m_controller;
    string m_host;
    int m_port;
    string m_currentGame;
    bool m_fullscreen;

public:
    Prefs(string filename) : m_filename(filename),m_engine(),m_controller(),m_currentGame(),m_fullscreen(false) {}

    bool read() {
        FILE* fp=fopen(m_filename.c_str(),"r");
        if(!fp) return false;
        json j=json::parse(fp);
        m_engine=j["engine"];
        m_controller=j["controller"];
        m_currentGame=j["currentgame"];
        m_fullscreen=j["fullscreen"];
        fclose(fp);

        size_t found=m_controller.find(':');
        if(found!=string::npos) {
            m_host=m_controller.substr(0, found);
            m_port=atoi(m_controller.substr(found+1,string::npos).c_str());
        }
        return true;
    }
    bool isFullscreen() {return m_fullscreen;}
    const char* currentGame() {return m_currentGame.c_str();}
    const char* engine() {return m_engine.c_str();}
    const char* host() {return m_host.c_str();}
    int port() {return m_port;}

    // trim from start (in place)
    void ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
            return !std::isspace(ch);
        }));
    }

    // trim from end (in place)
    void rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
            return !std::isspace(ch);
        }).base(), s.end());
    }

    // trim from both ends (in place)
    void trim(std::string &s) {
        rtrim(s);
        ltrim(s);
    }
};


#endif //CONTROLLER_GUI_PREFS_H

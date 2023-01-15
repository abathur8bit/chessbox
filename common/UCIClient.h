/* *****************************************************************************
 * Created by Lee Patterson 1/23/2021
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

#ifndef CONTROLLER_GUI_UCICLIENT_H
#define CONTROLLER_GUI_UCICLIENT_H

#include <string>
#include <mutex>
#include <list>
#include <map>

#include "process.hpp"

using namespace TinyProcessLib;
using namespace std;

void uciClear();
void uciPush(string s);
void uciWait();
string uciPull();
void uciParse(char *dest, int size, shared_ptr<string> buffer);
bool uciIsDebug();
void uciSetDebug(bool debug);

enum OptionType {option_unknown,option_string, option_spin, option_button, option_check, option_combo};
#define ENGINE_OPTION_DEFAULT "default"
#define ENGINE_OPTION_MIN "min"
#define ENGINE_OPTION_MAX "max"
#define ENGINE_OPTION_VAR "var"
#define ENGINE_OPTION_DEPTH "depth"
#define ENGINE_OPTION_MOVETIME "movetime"
#define ENGINE_OPTION_SKILL_LEVEL "Skill Level"

class EngineOption {
public:
    EngineOption(string name,OptionType type) : m_name(name),m_type(type) {};

    const char* name() {return m_name.c_str();}
    OptionType type(){return m_type;}
    const char* typeName() {
        switch(m_type) {
            case option_string: return "string"; break;
            case option_spin: return "spin"; break;
            case option_button: return "button"; break;
            case option_check: return "check"; break;
            default: return "unknown"; break;
        }
    }
    virtual const char* currentValue()=0;
    virtual const char* defaultValue()=0;

    string m_name;
    OptionType m_type;
};

class EngineStringOption : public EngineOption {
public:
    EngineStringOption(string name,string defaultValue,string currentValue) : EngineOption(name,option_string),m_defaultValue(defaultValue),m_currentValue(currentValue) {}
    const char* currentValue() {return m_currentValue.c_str();}
    const char* defaultValue() {return m_defaultValue.c_str();}

protected:
    string m_defaultValue;
    string m_currentValue;
};

class EngineCheckOption : public EngineOption {
public:
    EngineCheckOption(string name,bool defaultValue,bool currentValue) : EngineOption(name,option_check),m_defaultValue(defaultValue),m_currentValue(currentValue){}
    const char* currentValue() {return m_currentValue ? "true":"false";}
    const char* defaultValue() {return m_defaultValue ? "true":"false";}
    bool isChecked(){return m_currentValue;}
protected:
    bool m_defaultValue;
    bool m_currentValue;
};

class EngineSpinOption : public EngineOption {
public:
    EngineSpinOption(string name,int min,int max,int defaultValue,int currentValue) : EngineOption(name,option_spin),m_min(min),m_max(max),m_defaultValue(defaultValue),m_currentValue(currentValue) {}
    const char* toString(int n) {
        snprintf(m_buffer,sizeof m_buffer,"%d",m_currentValue);
        return m_buffer;
    }
    const char* currentValue() {
        return toString(m_currentValue);
    }
    const char* defaultValue() {
        return toString(m_defaultValue);
    }
    int minValue(){return m_min;}
    int maxValue(){return m_max;}
    int m_min,m_max,m_defaultValue,m_currentValue;
    char m_buffer[1024];

};

class EngineComboOption : public EngineOption {
public:
    EngineComboOption(string name,string defaultValue,string currentValue) : EngineOption(name,option_combo),m_vars(),m_defaultValue(defaultValue),m_currentValue(currentValue) {}
    const char* currentValue() {return m_currentValue.c_str();}
    const char* defaultValue() {return m_defaultValue.c_str();}
    void push_back(string var){m_vars.push_back(var);}
    list<string>::iterator begin(){return m_vars.begin();}
    list<string>::iterator end(){return m_vars.end();}
    string m_defaultValue;
    string m_currentValue;
    list<string> m_vars;
};

//todo EngineButtonOption

class UCIClient {
public:
    UCIClient() : m_enginePath(""), m_pProcess(nullptr),m_debug(false),m_options() {}
    UCIClient(string enginePath) : m_enginePath(enginePath), m_pProcess(nullptr),m_debug(false),m_options() {}
    bool start();
    void stop();
    void newGame();
    void setPosition(string fen);
    string bestMove(string fen);
    void bestMove(string& move,string& ponder,string fen);
    void sendCommand(string s);
    void discoverOptions();
    const char* engineName();
    EngineOption* option(string key) {return m_options[key];}
    void setSpinOption(string key,int value);
    void setLevel(int level);   ///< Sets the skill level, movetime and depth. Skill levels from 0-8.
    void setDepth(int depth);
    void setMovetime(int movetime);
    string waitFor(string s);
    void setDebug(bool debug){uciSetDebug(debug);}
    bool isDebug(){return uciIsDebug();}
    string pull() {return uciPull();}
    void clear() {uciClear();}
    EngineOption* parseOption(string line);
    EngineSpinOption* parseSpinOption(string name,OptionType type,string line);
    EngineStringOption* parseStringOption(string name,OptionType type,string line);
    EngineCheckOption* parseCheckOption(string name,OptionType type,string line);
    EngineComboOption* parseComboOption(string name,OptionType type,string line);
    OptionType stringToType(string stype);

protected:
    string m_enginePath;
    Process* m_pProcess;
    bool m_debug;
    map<string,EngineOption*> m_options;
};


#endif //CONTROLLER_GUI_UCICLIENT_H

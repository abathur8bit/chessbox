//
// Created by patte on 12/29/2020.
//

#ifndef CONTROLLER_GUI_UCICLIENT_H
#define CONTROLLER_GUI_UCICLIENT_H

#include <string>
#include <mutex>
#include <list>
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

class UCIClient {
public:
    UCIClient(string enginePath) : m_enginePath(enginePath), m_pProcess(nullptr),m_debug(false) {}
    bool start();
    void stop();
    void newGame();
    void setPosition(string fen);
    string bestMove(string fen,long time=1);
    void bestMove(string& move,string& ponder,string fen,long time=1);
    void sendCommand(string s);
    string waitFor(string s);
    void setDebug(bool debug){uciSetDebug(debug);}
    bool isDebug(){return uciIsDebug();}
    string pull() {return uciPull();}
    void clear() {uciClear();}
protected:
    string m_enginePath;
    Process* m_pProcess;
    bool m_debug;
};


#endif //CONTROLLER_GUI_UCICLIENT_H

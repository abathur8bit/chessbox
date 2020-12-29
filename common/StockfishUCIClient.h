//
// Created by patte on 12/29/2020.
//

#ifndef CONTROLLER_GUI_STOCKFISHUCICLIENT_H
#define CONTROLLER_GUI_STOCKFISHUCICLIENT_H

#include <string>
#include <mutex>
#include <list>
#include "process.hpp"
using namespace TinyProcessLib;
using namespace std;

class StockfishUCIClient {
public:
    StockfishUCIClient(string filepath);
    bool start();
    void stop();
    void newGame();
    void setPosition(string fen);
    void bestMove(string fen);
    void sendCommand(string s);

protected:
    string m_stockfishPath;
    Process* m_pProcess;
};


#endif //CONTROLLER_GUI_STOCKFISHUCICLIENT_H

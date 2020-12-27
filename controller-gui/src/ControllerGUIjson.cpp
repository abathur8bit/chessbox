//
// Created by patte on 12/27/2020.
//
#include <string>
#include <chessaction.hpp>

#include "ControllerGUI.h"

#include "json.hpp"

using namespace std;
using namespace nlohmann;   //trying this

void ControllerGUI::processJson(const char* buffer) {
    json j = json::parse(buffer);
    if(j.contains("action")) {
        string action = j["action"];
        if(!action.compare("pieceUp")) {
            string square=j["square"];
            m_board->highlightSquare(m_board->toIndex(square.c_str()),true);
//            invalidate();
        } else if(!action.compare("pieceDown")) {
            string square=j["square"];
            m_board->highlightSquare(m_board->toIndex(square.c_str()),false);
//            invalidate();
        } else if(!action.compare("move")) {
            ChessAction* a=new ChessAction(j);
            ChessMove m = a->move(0);
            printf("Move from=%s to=%s lan=%s\n",m.from(),m.to(),m.lan());
            m_board->playMove(m.lan());
//            invalidate();
        } else if(!action.compare("setposition")) {
            string fen=j["fen"];
            m_board->Forsyth(fen.c_str());
            m_rules.display_position();
            m_movesPanel->clear();
//            invalidate();
        }
    }
}
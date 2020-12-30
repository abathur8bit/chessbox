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
            printf("board is now [%s]\n",m_board->rules()->ForsythPublish().c_str());
            string bestMove = m_uci.bestMove(m_board->rules()->ForsythPublish());   //returns move in LAN format "a2a3"
            printf("computer plays [%s]\n",bestMove.c_str());
            m_board->playMove(bestMove.c_str());
            char from[3],to[3];
            m_board->fromTo(bestMove.c_str(),from,to);
            vector<json> moves;
            json move;
            move["from"]=from;
            move["to"]=to;
            move["type"]="move";
            moves.push_back(move);
            json toSend;
            toSend["action"]="move";
            toSend["description"]=nullptr;
            toSend["moves"]=moves;
            m_connector->send(toSend.dump().c_str());
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
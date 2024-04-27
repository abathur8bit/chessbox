//
// Created by patte on 12/27/2020.
//
#include <string>
#include <chessaction.hpp>

#include "ControllerGUI.h"
#include "PGNUtils.h"
#include "json.hpp"
#include "Dialog.h"

using namespace std;
using namespace nlohmann;   //json

void ControllerGUI::processJson(const char* buffer) {
    json j = json::parse(buffer);
    if(j.contains("action")) {
        string action = j["action"];
        if(!action.compare("piece_up")) {
            string square=j["square"];
            m_board->highlightSquare(m_board->toIndex(square.c_str()),true);
//            invalidate();
        } else if(!action.compare("piece_down")) {
            string square=j["square"];
            m_board->highlightSquare(m_board->toIndex(square.c_str()),false);
//            invalidate();
        } else if(!action.compare("move")) {
            ChessAction* a=new ChessAction(j);
            ChessMove m = a->move(0);
            Move mv;
            mv.TerseIn(m_board->rules(),m.lan());
            string san = mv.NaturalOut(m_board->rules());
            printf("Move from=%s to=%s lan=%s san=%s\n",m.from(),m.to(),m.lan(),san.c_str());
            m_board->playMove(m.lan());
            printf("board is now [%s]\n",m_board->rules()->ForsythPublish().c_str());
            if(m_board->rules()->isMate()) {
                saveGame();
                string buffer=san;
                buffer+="#";
                m_movesPanel->add(buffer.c_str());
            } else {
                string bestMove=m_uci.bestMove(m_board->rules()->ForsythPublish());   //returns move in LAN format "a2a3"
                Move mvBest;
                mvBest.TerseIn(m_board->rules(),bestMove.c_str());
                string bestMoveSan=mvBest.NaturalOut(m_board->rules());
                printf("computer plays [%s]\n", bestMoveSan.c_str());
                m_board->playMove(bestMove.c_str());
                char from[3], to[3];
                m_board->fromTo(bestMove.c_str(), from, to);
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
                snprintf(m_buffer,sizeof m_buffer,"%d. %-5s %-5s",m_board->rules()->full_move_count-1,san.c_str(),bestMoveSan.c_str());
                saveGame();
                m_movesPanel->add(m_buffer);

//            invalidate();
            }
        } else if(!action.compare("setposition")) {
            string fen=j["fen"];
            m_board->Forsyth(fen.c_str());
            m_board->rules()->display_position();
            m_movesPanel->clear();
//            invalidate();
        }
    }
}

void ControllerGUI::saveGame() {
    PGNUtils pgn;
    string result=PGN_RESULT_NO_WIN;
    if(m_board->rules()->isWhiteMate()) result=PGN_RESULT_WHITE_WIN;
    if(m_board->rules()->isBlackMate()) result=PGN_RESULT_BLACK_WIN;
    string computerName="Chessbox ";
    computerName+=m_uci.engineName();
    pgn.save(m_pgnFile.c_str(),m_board->rules(),result,"Human",computerName);
}


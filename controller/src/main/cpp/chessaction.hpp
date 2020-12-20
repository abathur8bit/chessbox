//
// Created by patte on 8/30/2020.
//

#ifndef CONTROLLER_CHESSACTION_HPP
#define CONTROLLER_CHESSACTION_HPP
#include "json.hpp"
#include "chessmove.hpp"
#include <string>

using namespace nlohmann;   //trying this
//using json = nlohmann::json;

using namespace std;

class ChessAction {
protected:
    string m_action;
    ChessMove m_moves[10];
    int m_numMoves;
public:
    int numMoves() {return m_numMoves;}
    const char* action() {return m_action.c_str();}

    ChessMove& move(int i) {
        return m_moves[i];
    }
    ChessAction(json j) //: m_action(),m_moves(),m_numMoves(0)
    {
        m_numMoves=0;
        m_action = j["action"];
        auto s = j["moves"];
        if(s != nullptr) {
            int num = s.size();
            m_numMoves = num;
            for (int i = 0; i < num; i++) {
                m_moves[i] = ChessMove(s[i]);
//            printf("move %d = %s\n",i,m_moves[i].c_str());
            }
        }
    }
};

#endif //CONTROLLER_CHESSACTION_HPP

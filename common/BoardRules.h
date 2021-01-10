//
// Created by patte on 12/27/2020.
//

#ifndef CONTROLLER_GUI_BOARDRULES_H
#define CONTROLLER_GUI_BOARDRULES_H

#include <string>
#include "thc.h"

using namespace std;
using namespace thc;

/** Rules of chess. Add moves, and inspect history here. */
class BoardRules : public thc::ChessRules {
public:
    /** Move in simple algebraic notation, like "d4", "Nc6" or "Nxd4". */
    void playMove(string san) {
        Move mv;
        mv.NaturalIn(this,san.c_str());
        PlayMove(mv);
    }

    char pieceAt(int i) {
        return squares[i];
    }

    thc::Move historyAt(unsigned char index) {
        return history[index];
    }

    unsigned char historyIndex() {return history_idx;}

    void display_position()
    {
        std::string fen = ForsythPublish();
        std::string s = ToDebugStr();
        printf("FEN = %s\n", fen.c_str());
        printf("Position = %s\n", s.c_str());
    }

    bool isMoveValid(const char* lan) {
        thc::Move mv;
        return mv.TerseIn(this,lan);
    }


    /**
     * Returns true if the move is a checking move (ends with '+'. Note
     * you should do the check before calling PlayMove.
     *
     * @param mv Move before it is played.
     * @return true if it is a check, false otherwise.
     */
    bool isCheck(thc::Move mv) {
        return mv.NaturalOut(this).find_first_of('+') == string::npos ? false:true;
    }

    bool isMate() {
        thc::TERMINAL terminal;
        Evaluate(terminal);
        return (TERMINAL_WCHECKMATE==terminal || TERMINAL_BCHECKMATE==terminal);
    }

    bool isWhiteMate() {
        thc::TERMINAL terminal;
        Evaluate(terminal);
        return (TERMINAL_WCHECKMATE==terminal);
    }

    bool isBlackMate() {
        thc::TERMINAL terminal;
        Evaluate(terminal);
        return (TERMINAL_BCHECKMATE==terminal);
    }
};
#endif //CONTROLLER_GUI_BOARDRULES_H

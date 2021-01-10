/* *****************************************************************************
 * Created by Lee Patterson 1/2/2021
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

#include "UCIClient.h"
#include <map>
#include <typeinfo>
#include <iostream>
#include <BoardRules.h>
#include <PGNUtils.h>

using namespace thc;
using namespace std;

void replay(BoardRules& rules) {
    rules.playMove("d4");
    rules.playMove("d5");
    rules.playMove("c4");
    rules.playMove("Nc6");
    rules.playMove("Nc3");
    rules.playMove("Nf6");
    rules.playMove("Bg5");
    rules.playMove("e6");
    rules.playMove("Bxf6");
    rules.playMove("Qxf6");
    rules.playMove("e4");
    rules.playMove("Bb4");
    rules.playMove("cxd5");
    rules.playMove("Qxd4");
    rules.playMove("Qxd4");
    rules.playMove("Nxd4");
    rules.playMove("Bd3");
    rules.playMove("O-O");
    rules.playMove("a3");
    rules.playMove("Bd6");
    rules.playMove("Nge2");
    rules.playMove("Nb3");
    rules.playMove("Rd1");
    rules.playMove("Bd7");
    rules.playMove("O-O");
    rules.playMove("c5");
    rules.playMove("Bc4");
    rules.playMove("Na5");
    rules.playMove("b3");
    rules.playMove("Rad8");
    rules.playMove("f4");
    rules.playMove("e5");
    rules.playMove("f5");
    rules.playMove("f6");
    rules.playMove("Bb5");
    rules.playMove("Bxb5");
    rules.playMove("Nxb5");
    rules.playMove("c4");
    rules.playMove("b4");
    rules.playMove("Be7");
    rules.playMove("d6");
    rules.playMove("Kf7");
    rules.playMove("dxe7");
    rules.playMove("Rxd1");
    rules.playMove("Rxd1");
    rules.playMove("Ra8");
    rules.playMove("Rd7");
    rules.playMove("Ke8");
    rules.playMove("Rc7");
    rules.playMove("Kf7");
    rules.playMove("Nd6+");
    rules.playMove("Kg8");
    rules.playMove("e8=Q+");
    rules.playMove("Rxe8");
    rules.playMove("Nxe8");
    rules.playMove("Kf8");
    rules.playMove("Nxg7");
    rules.playMove("Nb3");
    rules.playMove("Ne6+");
    rules.playMove("Ke8");
    rules.playMove("Rxb7");
    rules.playMove("a5");
    rules.playMove("b5");
    rules.playMove("Nd2");
    rules.playMove("Nc3");
    rules.playMove("Nb1");
    rules.playMove("Nxb1");
    rules.playMove("c3");
    rules.playMove("Nxc3");
    rules.playMove("a4");
    rules.playMove("b6");
    rules.playMove("h5");
    rules.playMove("Ra7");
    rules.playMove("h4");
    rules.playMove("b7");
    rules.playMove("Ke7");
    rules.playMove("b8=Q");
}

void replayShortGame(BoardRules& rules) {
    rules.playMove("d3");
    rules.playMove("Nf6");
    rules.playMove("e4");
    rules.playMove("e5");
}
int main(int argc,char* argv[]) {
    const char* lan="b1c3";
    BoardRules rules;
    BoardRules pgnRules;
    replay(rules);
//    replayShortGame(rules);
    for(unsigned char i=1; i<rules.historyIndex(); i++) {
        Move historyMove = rules.historyAt(i);
        Move pgnMove;
        pgnMove.TerseIn(&pgnRules, historyMove.TerseOut().c_str());
        cout << "history " << historyMove.TerseOut() << " pgn move: " << pgnMove.NaturalOut(&pgnRules) << endl;
        pgnRules.playMove(historyMove.TerseOut().c_str());
    }

    PGNUtils pgn;
    string result=PGN_RESULT_NO_WIN;
    if(rules.isWhiteMate()) result=PGN_RESULT_WHITE_WIN;
    if(rules.isBlackMate()) result=PGN_RESULT_BLACK_WIN;
    pgn.save("testgame.pgn",&rules,result);
}



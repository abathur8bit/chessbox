#include <string>
#include <array>
#include <iostream>     // std::cout, std::ios
#include <sstream>
#include <fstream>

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "chessmove.hpp"
#include "BoardRules.h"
#include "json.hpp"
#include "PGNUtils.h"

using namespace nlohmann;   //trying this
using namespace std;
using namespace thc;

int toIndex(const char* square) {
    int col=toupper(square[0])-'A';
    int row=8-(square[1]-'0');
    printf("%s col=%d row=%d\n",square,col,row);
    return row*8+col;
}

void vectorTest() {
    std::vector<json> moves;
    json move;
    move["from"]="a2";
    move["to"]="a3";
    move["type"]="move";
    moves.push_back(move);
    move["from"]="h2";
    move["to"]="h3";
    move["type"]="capture";
    moves.push_back(move);

    json j;
    j["action"]="move";
    j["description"]= nullptr;
    j["moves"]=moves;
    printf("vector json=%s\n",j.dump().c_str());
}

void jsonTest() {
    char square[3]="A1";
    printf("index=%d %s\n",toIndex(square),square);
    strcpy(square,"A8");
    printf("index=%d %s\n",toIndex(square),square);
    strcpy(square,"H1");
    printf("index=%d %s\n",toIndex(square),square);
    strcpy(square,"H8");
    printf("index=%d %s\n",toIndex(square),square);

    json move;
    move["from"]="a2";
    move["to"]="a3";
    move["type"]="move";
    std::array<json,2> moveArray;
    moveArray[0]=move;
    move["from"]="h2";
    move["to"]="h3";
    move["type"]="capture";
    moveArray[1]=move;


    json j;
    j["action"]="move";
    j["description"]= nullptr;
    j["moves"]=moveArray;
    printf("json=%s\n",j.dump().c_str());

    printf("=========\n\n");
    vectorTest();

}

void moveTest() {
    BoardRules rules;
    rules.Forsyth("7k/5P2/8/5K2/8/7p/8/8 w - - 0 1");
    rules.display_position();
    Move mv;
    mv.TerseIn(&rules,"f7f8N");
    printf("move=%s\n",mv.TerseOut().c_str());
    rules.playMove("f7f8");
    rules.display_position();
    mv = rules.historyAt(1);
    printf("move after=%s\n",mv.TerseOut().c_str());
//    mv.TerseIn(&rules,"")

    json j = json::parse("{\"from\":\"b8\",\"to\":\"a6\",\"type\":\"move\"}");
    ChessMove chessMove(json::parse("{\"from\":\"b8\",\"to\":\"a6\",\"type\":\"move\"}"));
    printf("json=%s chessmove=%s index from=%d to=%d\n",j.dump().c_str(),chessMove.lan(),chessMove.fromIndex(),chessMove.toIndex());
    string s="a8";
    printf("square=%s index=%d\n",s.c_str(),chessMove.squaresIndex(s));
    s="a1"; printf("square=%s index=%d\n",s.c_str(),chessMove.squaresIndex(s));

}

void pgnfen() {
    string fen="7k/3RP3/4K2p/8/8/8/8/8 w - - 0 1";
    const char* pgnfile="/t/test.pgn";
    BoardRules rules;
//    rules.playMove("e4");
//    rules.playMove("e5");
//    rules.playMove("d4");
//    rules.playMove("d6");
//    rules.playMove("d5");

//    rules.playMove("d3");
//    rules.playMove("e6");
//    rules.playMove("e4");
//    rules.playMove("Bb4+");

    rules.Forsyth(fen.c_str());
    rules.playMove("e8=N");
    rules.display_position();
    if(rules.isMate()) {
        printf("final position is check mate\n");
    }

    PGNUtils pgn;
    pgn.save(pgnfile,&rules,PGN_RESULT_NO_WIN,"Lee Patterson","Chessbox Stockfish",1,PGN_EMPTY,"Chessbox",PGN_EMPTY,fen);
    printf("done exporting to %s history index=%d\n",pgnfile,rules.historyIndex());
}

void pgnfenmatewhite() {
    BoardRules rules;
    const char* fen="7k/3RQ2r/4K3/7r/8/8/2q5/8 w - - 0 1";
    const char* pgnfile="/t/test.pgn";

    rules.Forsyth(fen);
    rules.playMove("Qe8");
    rules.display_position();
    printf("is mate:%d\n",rules.isMate());

    PGNUtils pgn;
    pgn.save(pgnfile,&rules,PGN_RESULT_WHITE_WIN,"Lee Patterson","Chessbox Stockfish",1,PGN_EMPTY,"Chessbox");
    printf("done exporting to %s history index=%d\n",pgnfile,rules.historyIndex());
}
void pgnmate() {
    BoardRules rules;
    const char* pgnfile="/t/test.pgn";

    rules.playMove("c3");
    rules.playMove("e6");
    rules.playMove("f3");
    rules.playMove("d5");
    rules.playMove("g4");
//    rules.playMove("Qh4#");
    rules.display_position();

    PGNUtils pgn;
    pgn.save(pgnfile,&rules,PGN_RESULT_BLACK_WIN,"Lee Patterson","Chessbox Stockfish",1,PGN_EMPTY,"Chessbox");
    printf("done exporting to %s history index=%d\n",pgnfile,rules.historyIndex());
}

void sendToStream(ostream& out) {
    out << "test the foobar" << endl;
}
void fileStream() {
    filebuf fb;
    fb.open ("test.txt",std::ios::out);
    std::ostream os(&fb);
    sendToStream(os);
    fb.close();
}

void stringStream() {
    ostringstream foo;
    sendToStream(foo);
    cout << "foo contains " << foo.str() << endl;
}
void streams() {
    fileStream();
    stringStream();
}

void savePgn() {
    BoardRules rules;
    rules.playMove("a4");
    rules.playMove("a5");
    PGNUtils pgn;
    pgn.save("test.pgn",&rules,"*","?","?",-1,"");
    ostringstream game;
    pgn.save(game,&rules,"*","?","?",-1,"2020.01.01");
    cout << game.str() << endl;
}
int main(int argc,char* argv[]) {
    printf("Hello World\n");

//    jsonTest();
//    processTest();
//    moveTest();

//    pgntest();
//    pgnmate();
//    pgnfenmatewhite();

    streams();
    savePgn();
    return 0;

}
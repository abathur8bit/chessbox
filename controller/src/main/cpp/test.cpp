#include <string>
#include <array>

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "chessmove.hpp"
#include "BoardRules.h"
#include "json.hpp"

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

int main(int argc,char* argv[]) {
    printf("Hello World\n");

//    jsonTest();
//    processTest();
    moveTest();
    return 0;
}
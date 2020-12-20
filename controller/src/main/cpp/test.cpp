#include <string>
#include <array>

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "json.hpp"
using namespace nlohmann;   //trying this

int toIndex(const char* square) {
    int col=toupper(square[0])-'A';
    int row=8-(square[1]-'0');
    printf("%s col=%d row=%d\n",square,col,row);
    return row*8+col;
}

void vector() {
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
    printf("vectory json=%s\n",j.dump().c_str());
}
int main(int argc,char* argv[]) {
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
    vector();
    return 0;
}
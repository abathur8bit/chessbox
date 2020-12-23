#include <iostream>

#include "chessmove.hpp"
#include "chessaction.hpp"
#include "json.hpp"

using json = nlohmann::json;
using namespace std;

class person {
    person(json j) {
        name = j["name"];
    }
    string name;
    int age;

};

ChessAction* parseJson(const char* s) {
    json j = json::parse(s);
    ChessAction* c = new ChessAction(j);
    return c;
}

int main() {
//    const char* jsonString = "{\"action\":\"move\",\"description\":\"Castle short\",\"moves\":[{\"m_from\":\"e1\",\"to\":\"g1\",\"capture\":true},{\"m_from\":\"h1\",\"to\":\"f1\",\"capture\":false}]}";
    const char* jsonString = "{\"action\":\"move\",\"description\":\"Castle short\",\"moves\":[{\"m_from\":\"e1\",\"to\":\"g1\",\"m_type\":\"move\"},{\"m_from\":\"h1\",\"to\":\"f1\",\"m_type\":\"move\"}]}";
    std::cout << "Hello, World!" << std::endl;
    json j = json::parse(jsonString);
    cout << j << endl;

//    string action = j["action"];
//    printf("action=%s\n",action.c_str());
//    ChessAction c(j);
//    cout << "action=" << c.action << " square=" << c.square << endl;

    ChessAction* ca = parseJson(jsonString);
    printf("action %s num moves=%d\n",ca->action(),ca->numMoves());
    for(int i=0; i<ca->numMoves(); i++)
    {
        printf("move %d = %s index m_from = %d\n",i,ca->move(i).c_str(),ca->move(i).fromIndex());
        printf("row %c col %c\n",ca->move(i).toRow(0),ca->move(i).toCol(0));
        printf("row %c col %c\n",ca->move(i).toRow(63),ca->move(i).toCol(63));

    }
    delete ca;

    return 0;
}

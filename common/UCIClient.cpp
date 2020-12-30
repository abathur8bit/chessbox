//
// Created by patte on 12/29/2020.
//

#include "UCIClient.h"
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iterator>
#include <cstring>
#include <condition_variable>

using namespace std;

list<string> queue;
mutex mtx;
condition_variable cv;
bool ready=false;
bool uciDebug=false;

bool uciIsDebug() {return uciDebug;}
void uciSetDebug(bool debug) {uciDebug=debug;}

void uciClear() {
    unique_lock<std::mutex> lck(mtx);
    queue.clear();
    ready=false;
}
void uciPush(string s) {
    unique_lock<std::mutex> lck(mtx);
    queue.push_back(s);
    ready=true;
    cv.notify_all();
}
//wait for a message to appear on the queue
void uciWait() {
    unique_lock<std::mutex> lck(mtx);
    while(!ready) cv.wait(lck);
}
//grab first message on the queue and return it, or return an empty string if empty
string uciPull() {
    unique_lock<std::mutex> lck(mtx);
    if(queue.empty())
        return "";
    string s=queue.front();
    queue.pop_front();
    if(queue.empty())
        ready=false;
    return s;
}
void uciParse(char *dest, int size, shared_ptr<string> buffer) {
    unsigned i;
    int destIdx=0;
    for(i=0; i<buffer->length(); i++) {
        if('\r'==buffer->at(i)) {
            continue;
        } else if('\n'==buffer->at(i)) {
            //found end of line
            dest[destIdx++]='\0';
            buffer->erase(0,i+1);
            break;
        } else if(size-1==i) {
            //dst buffer size will be exceeded
            dest[destIdx++]='\0';
            buffer->erase(0,i);
            break;
        }
        dest[destIdx++]=buffer->at(i);
    }
    if(i==buffer->length()) {
        dest[0]='\0';   //we didn't find a new line, returning an empty string
    }
}


/** Starts and connects to the UCI engine. */
bool UCIClient::start() {
    auto output = make_shared<string>();
    auto error = make_shared<string>();

    m_pProcess = new Process(m_enginePath, "", [output](const char *bytes, size_t n) {
        char line[1024];
        *output += string(bytes, n);
        do {
            uciParse(line, sizeof line, output);
            if(strlen(line)) {
                uciPush(line);
                if(uciIsDebug()) {
                    printf("read line [%s]\n", line);
                }
            }
        } while(strlen(line));
    }, nullptr, true);
    return true;
}
/** Tells the engine to start a new game. */
void UCIClient::newGame() {
    sendCommand("ucinewgame");
}
/** Tells the engine to stop looking for a move, quits the engine and closes the streams. */
void UCIClient::stop() {
    sendCommand("stop");
    sendCommand("quit");
    m_pProcess->get_exit_status();  //waits for process to exit
}
/** \brief Sends a position fen command to the engine.
 * @param fen The position to setup. */
void UCIClient::setPosition(string fen) {
    sendCommand("position fen "+fen);
}
/** \brief Asks engine for the best move for the given position.
 * Sends a "position fen" command followed by a "go movetime <time>".
 * Shredder needs a move time, or it goes on forever.
 *
 * @param fen The position to get the move from.
 * @param time Time in milliseconds to evaluate.
 * @return The move in LAN format, ie: "b8c6".
 */
string UCIClient::bestMove(string fen, long time) {
    setPosition(fen);
    char buffer[1024];
    snprintf(buffer,sizeof(buffer),"go movetime %d",time);
    sendCommand(buffer);
    string move=waitFor("bestmove");
    std::istringstream buf(move);
    std::istream_iterator<std::string> beg(buf), end;
    std::vector<std::string> tokens(beg, end); // done!
    if(uciIsDebug()) {
        for(auto &s: tokens)
            std::cout << '"' << s << '"' << '\n';
    }
    return tokens[1];
}

void UCIClient::bestMove(string& move, string& ponder, string fen, long time) {
    setPosition(fen);
    char buffer[1024];
    snprintf(buffer,sizeof(buffer),"go movetime %d",time);
    sendCommand(buffer);
    string best=waitFor("bestmove");
    std::istringstream buf(best);
    std::istream_iterator<std::string> beg(buf), end;
    std::vector<std::string> tokens(beg, end); // done!
    if(uciIsDebug()) {
        printf("Tokens on wanted line:\n");
        for(auto &s: tokens)
            std::cout << '"' << s << '"' << '\n';
    }
    move=tokens[1];
    if(best.find("ponder")!=string::npos) {
        ponder=tokens[3];
    }
}
/** \brief Sends a command to the engine asynchronously. A new line is added to the string before it is sent.*/
void UCIClient::sendCommand(string s) {
    if(uciIsDebug()) {
        cout << "sending command [" << s << "]" << endl;
    }
    s+="\n";
    m_pProcess->write(s);
}
/** \brief Keeps parsing lines, ignoring lines that don't match, until the given string is found. */
string UCIClient::waitFor(string s) {
    string lineWanted;
    do {
        uciWait();
        string line=uciPull();
        if(line.find(s)!=string::npos) {
            if(uciIsDebug())
                cout << "found wanted line ["<<line<<"]"<<endl;
            lineWanted=line;
        } else if(uciIsDebug()) {
            cout << "ignoring line [" << line << "]" << endl;
        }
    } while(lineWanted.empty());
    return lineWanted;
}
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
    if(queue.empty()) {
        return "";
    }
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

    if(isDebug()) {
        printf("connecting to uci engine at %s\n",m_enginePath.c_str());
    }
    //check if engine exists
    FILE* fp = fopen(m_enginePath.c_str(),"r");
    if(!fp) {
        printf("ERROR: engine at %s doesn't exist\n",m_enginePath.c_str());
        return false;
    }
    fclose(fp);

    m_pProcess = new Process(m_enginePath, "", [output](const char *bytes, size_t n) {
        char line[1024];
        *output += string(bytes, n);
//        if(uciIsDebug()) {
//            cout << "read bytes ["<<*output<<"]" << endl;
//        }
        bool keepParsing=true;
        do {
            uciParse(line, sizeof line, output);
            if(uciIsDebug()) {
                printf("read line [%s]\n", line);
            }
            if(strlen(line)) {
                uciPush(line);
            } else {
                //make sure we are able to continue parsing after a blank line
                if(output->length()==0)
                    keepParsing=false;
            }
        } while(keepParsing);
    }, nullptr, true);
    return true;
}
/**
 * Special set level method so we can decide what other tweaks to gameplay we want to make
 * for a particular level.
 * @param level The Skill Level, between 0-20 inclusive.
 */
void UCIClient::setLevel(int level) {
    setSpinOption("Skill Level",level);
}
void UCIClient::setDepth(int depth) {
    EngineSpinOption* op;
    if(m_options.count(ENGINE_OPTION_DEPTH)) {
        //set options new value
        op=static_cast<EngineSpinOption *>(m_options[ENGINE_OPTION_DEPTH]);
        op->m_currentValue=depth;
    } else {
        //create the option
        op=new EngineSpinOption(ENGINE_OPTION_DEPTH,0,100,0,depth);
        m_options[ENGINE_OPTION_DEPTH]=op;
    }
}
void UCIClient::setMovetime(int ms) {
    EngineSpinOption* op;
    if(m_options.count(ENGINE_OPTION_MOVETIME)) {
        op=static_cast<EngineSpinOption *>(m_options[ENGINE_OPTION_MOVETIME]);
        op->m_currentValue=ms;
    } else {
        op=new EngineSpinOption(ENGINE_OPTION_MOVETIME,0,10000,0,ms);
        m_options[ENGINE_OPTION_MOVETIME]=op;
    }
}

void UCIClient::setSpinOption(string key,int value) {
    if(m_options.count(key)) {
        EngineSpinOption* op=static_cast<EngineSpinOption*>(m_options[key]);
        if(value>=op->minValue() && value<=op->maxValue()) {
            op->m_currentValue=value;
            char buffer[1024];
            snprintf(buffer,sizeof buffer,"setoption name %s value %d",op->name(),value);
            sendCommand(buffer);
        }
    }
}

void UCIClient::discoverOptions() {
    sendCommand("uci");
    string line;
    string idname="id name";
    do {
        uciWait();
        line=uciPull();
        if(line.find("option") != string::npos) {
            EngineOption* op=parseOption(line);
            if(op)
                m_options[op->name()]=op;
        } else if(line.find(idname) != string::npos) {
            //create a string option for the engine name
            string name=line.substr(idname.length()+1);
            EngineStringOption* op=new EngineStringOption("name",name,name);
            m_options[op->name()]=op;
        }

    } while(line.compare("uciok"));
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
string UCIClient::bestMove(string fen) {
    string move,ponder;
    bestMove(move,ponder,fen);
    return move;
}

void UCIClient::bestMove(string& move, string& ponder, string fen) {
    char buffer[1024];
    string command="go";
    if(m_options.count(ENGINE_OPTION_DEPTH)) {
        EngineSpinOption* op=static_cast<EngineSpinOption*>(m_options[ENGINE_OPTION_DEPTH]);
        int depth=op->m_currentValue;
        if(depth>0) {
            snprintf(buffer, sizeof(buffer), " %s %d",ENGINE_OPTION_DEPTH,depth);
            command+=buffer;
        }
    }
    if(m_options.count(ENGINE_OPTION_MOVETIME)) {
        EngineSpinOption* op=static_cast<EngineSpinOption*>(m_options[ENGINE_OPTION_MOVETIME]);
        int movetime=op->m_currentValue;
        if(movetime>0) {
            snprintf(buffer, sizeof(buffer), " movetime %d", movetime);
            command+=buffer;
        }
    }
    setPosition(fen);
    sendCommand(command);
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

OptionType UCIClient::stringToType(string stype) {
    if(!stype.compare("spin")) {
        return option_spin;
    } else if(!stype.compare("string")) {
        return option_string;
    } else if(!stype.compare("button")) {
        return option_button;
    } else if(!stype.compare("check")) {
        return option_check;
    } else if(!stype.compare("combo")) {
        return option_combo;
    } else {
        return option_unknown;
    }
}

//string can be blank, or can be "<empty>", or can have a string
EngineStringOption* UCIClient::parseStringOption(string name,OptionType type,string line) {
    const int len=line.length();
    int start=strlen(ENGINE_OPTION_DEFAULT)+1;
    int end=(start>len ? start:len);    //end is either the length of the string, or if there is nothing after default then it's the same as start
    if(start>len) {
        start=end=0;
    }
    string sdefault=line.substr(start,end-start);
    EngineStringOption* op=new EngineStringOption(name,sdefault,sdefault);
    return op;
}

EngineSpinOption* UCIClient::parseSpinOption(string name,OptionType type,string line) {
    int defaultStart=line.find(ENGINE_OPTION_DEFAULT)+strlen(ENGINE_OPTION_DEFAULT)+1;
    int defaultEnd=defaultStart;
    while(line[defaultEnd]!=' ' && defaultEnd<(int)line.length()) {
        defaultEnd++;
    }
    string sdefault=line.substr(defaultStart,defaultEnd-defaultStart);

    int minStart=line.find(ENGINE_OPTION_MIN)+strlen(ENGINE_OPTION_MIN)+1;
    int minEnd=minStart;
    while(line[minEnd]!=' ' && minEnd<(int)line.length()) {
        minEnd++;
    }
    string smin=line.substr(minStart,minEnd-minStart);

    int maxStart=minEnd+5;  //2 spaces and length of "max"
    int maxEnd=maxStart;
    while(line[maxEnd]!=' ' && maxEnd<(int)line.length()) {
        maxEnd++;
    }
    string smax=line.substr(maxStart,maxEnd-maxStart);

    EngineSpinOption* op=new EngineSpinOption(name,atoi(smin.c_str()),atoi(smax.c_str()),atoi(sdefault.c_str()),atoi(sdefault.c_str()));
    return op;
}
//option_string, option_spin, option_button, option_check

EngineCheckOption* UCIClient::parseCheckOption(string name,OptionType type,string line) {
    const int len=line.length();
    int start=strlen(ENGINE_OPTION_DEFAULT)+1;
    int end=(start>len ? start:len);    //end is either the length of the string, or if there is nothing after default then it's the same as start
    if(start>len) {
        start=end=0;
    }
    string sdefault=line.substr(start,end-start);
    bool value=!sdefault.compare("false") ? false:true; //"false" is false, "true" or anything else is true
    return new EngineCheckOption(name,value,value);
}
EngineComboOption* UCIClient::parseComboOption(string name,OptionType type,string line) {
    const int len=line.length();
    int start=strlen(ENGINE_OPTION_DEFAULT)+1;
    int end=start;
    while(line[end]!=' ' && end<len) {
        end++;
    }
    string sdefault=line.substr(start,end-start);

    EngineComboOption* op=new EngineComboOption(name,sdefault,sdefault);
    start=end+5;    //start at value after ' var '
    while(start<len) {
        end=start;
        while(line[end]!=' ' && end<len) {
            end++;
        }
        string var=line.substr(start,end-start);
        if(var.compare("var")) {
            op->push_back(var); //only if token is not the 'var' tag
        }
        start=end+1;
    }
    return op;
}
EngineOption* UCIClient::parseOption(string line) {
    int nameStart=strlen("option name ");
    int itype=line.find("type");
    int typeStart=itype+5;
    int typeEnd=line.find(' ',typeStart);
    string name=line.substr(nameStart,itype-nameStart-1);
    string stype=line.substr(typeStart,typeEnd-typeStart);
//    printf("name=[%s] type=[%s]\n",name.c_str(),stype.c_str());
    OptionType type=stringToType(stype);
    string toparse=line.substr(typeEnd+1);
    switch(type) {
        case option_string: return parseStringOption(name,type,toparse); break;
        case option_spin: return parseSpinOption(name,type,toparse); break;
//        case option_button: return "button"; break;
        case option_check: return parseCheckOption(name,type,toparse); break;
        case option_combo: return parseComboOption(name,type,toparse); break;
    }
    return nullptr;
}


//
// Created by patte on 12/29/2020.
//

#include "StockfishUCIClient.h"

list<string> queue;
mutex mtx;
condition_variable cv;
bool ready=false;

void clear() {
    unique_lock<std::mutex> lck(mtx);
    queue.clear();
    ready=false;
}
void push(string s) {
    unique_lock<std::mutex> lck(mtx);
    queue.push_back(s);
    ready=true;
    cv.notify_all();
}
//wait for a message to appear on the queue
void wait() {
    unique_lock<std::mutex> lck(mtx);
    while(!ready) cv.wait(lck);
}
//grab first message on the queue and return it, or return an empty string if empty
string pull() {
    unique_lock<std::mutex> lck(mtx);
    if(queue.empty())
        return "";
    string s=queue.front();
    queue.pop_front();
    if(queue.empty())
        ready=false;
    return s;
}

StockfishUCIClient::StockfishUCIClient(string filepath) : m_stockfishPath(filepath),m_pProcess(nullptr) {

}
bool StockfishUCIClient::start() {
    auto output = make_shared<string>();
    auto error = make_shared<string>();

    m_pProcess = new Process(m_stockfishPath,"",[output](const char *bytes, size_t n) {
        *output = string(bytes, n);
        printf("output=[%s]\n",output->c_str());
        push(*output);
    }, nullptr,true);
}
void StockfishUCIClient::newGame() {
    sendCommand("ucinewgame");
}
void StockfishUCIClient::stop() {
    sendCommand("quit");
    m_pProcess->get_exit_status();  //waits for process to exit
}
void StockfishUCIClient::setPosition(string fen) {
    sendCommand("position fen "+fen);
}
void StockfishUCIClient::bestMove(string fen) {

}
//sends a command to Stockfish engine asynchronously.
void StockfishUCIClient::sendCommand(string s) {
    s+="\n";
    m_pProcess->write(s);
}

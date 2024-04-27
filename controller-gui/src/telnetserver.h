//
// Created by patte on 4/27/2024.
//

#ifndef CONTROLLER_GUI_TELNETSERVER_H
#define CONTROLLER_GUI_TELNETSERVER_H

#include "socketinstance.h"

using namespace ssobjects;

class TelnetServer
{

public:
    TelnetServer(const SockAddr& saBind);
    ~TelnetServer();

    virtual void processLine(SocketInstance sock,SockAddr& sa,char* line);
    virtual void connected(SocketInstance sock,SockAddr& sa);
    virtual void closed(SocketInstance sock,SockAddr& sa);
    virtual void idle();

    void runServer();
    void alnumOnly(char* s);    //clears the string s of any non alpha numeric characters. Copy's over s with the new string.
    int extractLine(char *dest,int size);   //return dest or NULL if there isn't a line to read
    void hex(const char* pch,int size);
    void println(SocketInstance sock, const char* s);

public:
    SocketInstance          m_sListen;
    SocketInstance          m_sClient;
    SockAddr                m_saServer;
    SockAddr                m_saClient;
    fd_set                  m_rset,m_wset;
    bool                    m_bRunning;
    char                    m_buffer[1024];
    int                     m_buffSize;
};

#endif //CONTROLLER_GUI_TELNETSERVER_H

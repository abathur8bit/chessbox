#include <stdio.h>
#include <ctype.h>
#include "telnetserver.h"

TelnetServer::TelnetServer(const SockAddr& saBind)  : m_sListen(), m_saServer(saBind), m_rset(), m_wset() {
#ifdef _WIN32
    WSADATA wsd;
    if(WSAStartup(0x0101,&wsd)!=0)
        throwGeneralException("unable to start windows socket layer");
#endif
}

TelnetServer::~TelnetServer() {
    m_sListen.close();
#ifdef WIN32
    WSACleanup();
#endif
}

void TelnetServer::processLine(SocketInstance sock,SockAddr& sa,char* line) {
//    printf("Swalloed data [%s] from client\n",line);
}

void TelnetServer::connected(SocketInstance sock,SockAddr& sa) {
//    printf("Swallowed connection open from %s\n",sa.dottedDecimal());
}

void TelnetServer::closed(SocketInstance sock,SockAddr& sa) {
//    printf("Swalloed connection closed from %s\n",sa.dottedDecimal());
}

void TelnetServer::idle() {}

void TelnetServer::startServer() {
    m_sListen.create();
    m_sListen.bind(m_saServer);
    m_sListen.listen();
    SocketInstance conn;
    m_sListen.accept(conn,m_saClient);
    connected(conn,m_saClient);
    conn.setExceptionOnReadClose(false);

    m_bRunning = true;
    m_buffSize=0;
    while(m_bRunning) {
        struct timeval tv;
        memset(&tv,0,sizeof(tv));
        tv.tv_usec = 100*1000; //millisecond to microsecond
        FD_ZERO(&m_wset);
        FD_ZERO(&m_rset);
        FD_SET(m_sListen,&m_rset);      //select on listening socket
        FD_SET(conn,&m_rset);
        const int n = select(0, &m_rset, NULL, NULL, &tv);
//            printf("select returned %d\n",n);
        if(n == SOCKET_ERROR) {
            int err = WSAGetLastError();
            if(err == WSAENOTSOCK) {
                closed(conn,m_saClient);
            } else {
                printf("Socket error %d\n", err);
            }
            m_bRunning = false;
        } else if(n > 0) {
//                printf("%d sockets ready\n", n);
            char buff[1024];
            memset(buff,0,sizeof(buff));
            int bytes = conn.recv(buff, sizeof(buff));
            if(bytes > 0) {
//                hex(buff, bytes);
                memcpy(m_buffer + m_buffSize, buff, bytes);
                m_buffSize+=bytes;
                int count=extractLine(buff, sizeof buff);
                if(count>0) {
                    alnumOnly(buff);
                    processLine(conn,m_saClient,buff);
                }
            } else if(bytes == 0) {
                //socket closed
                printf("Socket was closed on client side %s\n",m_saClient.dottedDecimal());
                conn.cleanup();
                closed(conn,m_saClient);
                m_bRunning = false;
            }
        } else {
            idle();
        }
    }

    conn.cleanup();
}

//clears the string s of any non alpha numeric characters. Copy's over s with the new string.
void TelnetServer::alnumOnly(char* s) {
    char buff[1024];
    int dest=0;
    memset(buff,0,sizeof buff);
    for(int i=0; i<strlen(s); i++) {
        if(isalnum(s[i])) {
            buff[dest++] = s[i];
        }
    }
    strcpy(s,buff);
}

//return dest or NULL if there isn't a line to read
int TelnetServer::extractLine(char *dest,int size) {
    char buff[1024];
    for(int i=0; i<m_buffSize; i++) {
        if(m_buffer[i] == '\r') {
            memcpy(dest,buff,i);    //copy what we just parsed to dest
            dest[i] = '\0';         //null terminate dest
            memmove(m_buffer,m_buffer+(i+1),m_buffSize-i-1);  //move what we have not processed to beginning of buffer
            m_buffSize -= i+1;                                //adjust buffer size
//                if(m_buffSize>0) {
//                    printf("Buffer now %d bytes and contains contains:\n",m_buffSize);
//                    hex(m_buffer, m_buffSize);
//                }
//                printf("returning dest >>>\n");
//                hex(dest,i+1);
//                printf("<<<\n");
            return i;
        }
        buff[i] = m_buffer[i];
    }
    return NULL;
}

void TelnetServer::hex(const char* pch,int size) {
    for(int i=0; i<size; i++) {
        char ch = *(pch+i);
        char c = isalnum(ch) != 0 ? ch : '.';
        printf("%02X (%03d) %c\n",ch,ch,c);
    }
}

void TelnetServer::sockPrint(SocketInstance sock,char* s) {
    sock.write(s,strlen(s));
}


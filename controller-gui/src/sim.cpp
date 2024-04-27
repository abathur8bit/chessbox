#include <stdio.h>
#include <ctype.h>
#include "telnetserver.h"

#ifdef WIN32
WSADATA m_wsd;              ///< WSA startup information (Windows only)
#endif

class Sim : public TelnetServer {
public:
    Sim(const SockAddr& saBind) : TelnetServer(saBind) {}
    virtual void connected(SocketInstance sock,SockAddr& sa) {
        printf("Got connectin from %s\n",sa.dottedDecimal());
        sockPrint(sock,"Hello good citizen\r\n");
    }
    virtual void processLine(SocketInstance sock,SockAddr& sa,char* line) {
        printf("Got [%s] from client\n",line);
        char buff[1024];
        snprintf(buff,sizeof buff,"OK [%s]\r\n",line);
        sockPrint(sock,buff);

        if(strcmp(line,"quit")==0) {
            sockPrint(sock,"Good bye\r\n");
            sock.close();
        }
    }
    virtual void idle() {

    }
};

int main(int argc,char* argv[]) {
    int port=9999;
#ifdef WIN32
    int err=WSAStartup(0x0101, &m_wsd);
    if(err != 0) {
        printf("unable to start windows socket layer error=%d\n", err);
        exit(1);
    }
#endif

    const char* msg = "Hello World\r\n";
    bool running=true;
    SockAddr saBind((ULONG)INADDR_ANY,port);
    Sim server(saBind);

    //test alnumOnly
//    char buff[1024] = "\nabc\r\n";
//    printf("Buffer:\n");
//    server.hex(buff,6);
//    server.alnumOnly(buff);
//    printf("Buffer now:\n");
//    server.hex(buff,6);

    //test extrat line
//    char buff[1024] = "abcd\r";
//    server.hex(buff,6);
//    strcpy(server.m_buffer,"bleh\rxyz\r\n1234\r");
//    server.m_buffSize = strlen(server.m_buffer);
//    server.hex(server.m_buffer,server.m_buffSize);
//
//    printf("extracting line\n");
//    int n = server.extractLine(buff,sizeof buff);
//    printf("we extracted %d bytes:\n",n);
//    server.hex(buff,strlen(buff));
//
//    printf("extracting line 2\n");
//    n = server.extractLine(buff,sizeof buff);
//    printf("we extracted %d bytes:\n",n);
//    server.hex(buff,strlen(buff));
//
//    printf("extracting no line\n");
//    n = server.extractLine(buff,sizeof buff);
//    printf("we extracted %d bytes:\n",n);
//    server.hex(buff,strlen(buff));

    server.startServer();
    printf("Done\n");
}

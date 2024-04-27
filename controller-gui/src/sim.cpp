#include <stdio.h>
#include <ctype.h>
#include "telnetserver.h"
#include "json.hpp"

#ifdef WIN32
WSADATA m_wsd;              ///< WSA startup information (Windows only)
#endif

using namespace nlohmann;   //json

class Sim : public TelnetServer {
protected:
    char m_buffer[1024];
public:
    Sim(const SockAddr& saBind) : TelnetServer(saBind) {}
    virtual void connected(SocketInstance sock,SockAddr& sa) {
        printf("Connection from %s opened\n",sa.dottedDecimal());
        json j;
        j["message"] = "Hello";
        println(sock, j.dump().c_str());
    }
    virtual void closed(SocketInstance sock,SockAddr& sa) {
        printf("Connection from %s closed\n",sa.dottedDecimal());
    }
    virtual void processLine(SocketInstance sock,SockAddr& sa,char* line) {
        printf("Got [%s] from client\n",line);
        try {
            json j=json::parse(line);
            if(j.contains("action")) {
                string action=j["action"];
                snprintf(m_buffer, sizeof m_buffer, "OK [%s]", action.c_str());
                println(sock, m_buffer);

                if(action.compare("quit")==0) {
                    println(sock, "Good bye");
                    sock.close();
                }
            } else {
                json j;
                j["error"] = "missing action";
                println(sock,j.dump().c_str());
            }
        } catch (json::exception& ex) {
            printf("parse error at byte %s\n",ex.what());
            json j;
            j["error"] = "invalid json";
            println(sock,j.dump().c_str());
            sock.close();
        }

    }
    virtual void idle() {

    }
};

int main(int argc,char* argv[]) {
    int port=9999;
    SockAddr saBind((ULONG)INADDR_ANY,port);
    Sim server(saBind);
    server.runServer();
    printf("Done\n");
}

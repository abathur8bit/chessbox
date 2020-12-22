/* *****************************************************************************
 * Created by Lee Patterson 12/21/2020
 *
 * Copyright 2019 Lee Patterson <https://github.com/abathur8bit>
 *
 * You may use and modify at will. Please credit me in the source.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * ******************************************************************************/

#include "Connector.h"

#ifdef WIN32
bool Connector::m_bWSAStarted=false;
#endif

Connector::Connector() {
#ifdef WIN32
    if(!m_bWSAStarted)
    {
        printf("wsastartup\n");
        int err=WSAStartup(0x0101, &m_wsd);
        if(err != 0)
            printf("unable to start windows socket layer error=%d\n", err);
        m_bWSAStarted = true;
    }
#endif
}

int Connector::open(const char *host, unsigned short port) {
    int iResult;
    // Create a SOCKET for connecting to server
    SOCKET ConnectSocket;
    ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ConnectSocket == INVALID_SOCKET) {
        printf("socket function failed with error: %ld\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }
    // The sockaddr_in structure specifies the address family,
    // IP address, and port of the server to be connected to.
    sockaddr_in clientService = {0,0,0};
    clientService.sin_family = AF_INET;
    clientService.sin_addr.s_addr = inet_addr(host);
    clientService.sin_port = htons(port);

    // Connect to server.
    iResult = connect(ConnectSocket, (SOCKADDR *) & clientService, sizeof (clientService));
    if (iResult == SOCKET_ERROR) {
        printf("connect function failed with error: %ld\n", WSAGetLastError());
        iResult = closesocket(ConnectSocket);
        if (iResult == SOCKET_ERROR)
            printf("closesocket function failed with error: %ld\n", WSAGetLastError());
#ifdef WIN32
        WSACleanup();
#endif
        return 1;
    }

    printf("Connected to server.\n");

    iResult = closesocket(ConnectSocket);
    if (iResult == SOCKET_ERROR) {
        printf("closesocket function failed with error: %ld\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

#ifdef WIN32
    WSACleanup();
#endif
    return 0;
}

int Connector::open2(const char* host,unsigned short port) {
    printf("Creating socket\n");
    ssobjects::SocketInstance sock;
    printf("Connecting to %s:%d\n",host,port);
    sock.connect(host,port);
    char buffer[255];
    printf("Recieving data\n");
    int n=sock.recv(buffer,sizeof(buffer),60);
    buffer[n]=NULL;
    printf("read=%s\n",buffer);
    snprintf(buffer,sizeof(buffer),"{\"action\":\"ping\"}\r\n");
    n=sock.send(buffer,strlen(buffer),1);
    printf("sent %d of %d bytes\n",n,strlen(buffer));
    printf("Recieving data\n");
    n=sock.recv(buffer,sizeof(buffer),60);
    buffer[n]=NULL;
    printf("read=%s\n",buffer);
    return 0;
}

void Connector::open3(const char* host,unsigned short port) {
    printf("Creating socket\n");
    ssobjects::SocketInstance sock;
    printf("Connecting to %s:%d\n",host,port);
    sock.connect(host,port);
    char buffer[255]="";

    int ready=0;
    while(!ready) {
        printf("Waiting for data\n");

        fd_set rset;
        FD_ZERO(&rset);
        FD_SET(sock,&rset);

        struct timeval tv={0,0};
        ready=select(sock+1,&rset,NULL,NULL,&tv);
        if(-1 == ready) {
            printf("select error %d\n",ready);
        }
    }


    printf("Recieving data\n");
    int n=sock.recv(buffer,sizeof(buffer),60);
    buffer[n]=NULL;
    printf("read=%s\n",buffer);
    snprintf(buffer,sizeof(buffer),"{\"action\":\"ping\"}\r\n");
    n=sock.send(buffer,strlen(buffer),1);
    printf("sent %d of %d bytes\n",n,strlen(buffer));
    printf("Recieving data\n");
    n=sock.recv(buffer,sizeof(buffer),60);
    buffer[n]=NULL;
    printf("read=%s\n",buffer);

//
//
//    fd_set rset,wset,*pwset=NULL;
//    FD_ZERO(&rset);
//    FD_ZERO(&wset);
//
//    FD_SET(*m_pSocket,&rset);
}
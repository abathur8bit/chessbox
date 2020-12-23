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
#include <winsock.h>

#ifdef WIN32
bool Connector::m_bWSAStarted=false;
#endif

Connector::Connector() : m_sock(),m_bufferIndex(0) {
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
    iResult = ::connect(ConnectSocket, (SOCKADDR *) & clientService, sizeof (clientService));
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
    SocketInstance sock;
    printf("Connecting to %s:%d\n",host,port);
    sock.connect(host,port);
    printf("Connected\n");
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

void Connector::connect(const char *host, unsigned short port) {
    m_sock.connect(host,port);
}

/** Reads data until it gets a full line of data. A full line is ended by a \n char sequence. If less then a full line of data is read, it's buffered.
 * If there is a full line the next call, then the line is copied to buffer, null terminated, and \n is removed. If the buffer is too small to fit the
 * entire line, only size-n bytes are copied, buffer is null terminated, and the rest of the line is tossed. If a full line of data exceeds CONNECTOR_BUFFER_SIZE
 * we will keep reading, but will toss the excess data. A null terminated line containing the entire buffer would be copied to dest if dest is large enough.
 *
 * @param buffer
 * @param size     Max size of the buffer.
 * @return The number of bytes read, 0 if none read, -1 if there was an error.
 */
int Connector::readline(char* dest,size_t size) {
    //check if there is any data to read
    fd_set rset;
    FD_ZERO(&rset);
    FD_SET(m_sock,&rset);

    struct timeval tv={0,0};
    int ready=select(m_sock+1,&rset,NULL,NULL,&tv);
#
    if(-1 == ready) {
        printf("select error %d\n",ready);
        return -1;
    } else if(ready>0) {

        //read any new data
        char buf[1000];
        int n = m_sock.recv(buf, sizeof(buf));
        int max = n;
        if (m_bufferIndex + n >= CONNECTOR_BUFFER_SIZE) {
            max = CONNECTOR_BUFFER_SIZE - m_bufferIndex;
        }
        if (max) {
            memcpy(m_buffer + m_bufferIndex, buf, max);
            m_bufferIndex += max;
        }
    }

    //parse any existing data into lines
    int i;
    for(i=0; i<m_bufferIndex; i++) {
        if('\n'==m_buffer[i]) {
            //found end of line
            dest[i]='\0';
            memmove(m_buffer,m_buffer+i+1,m_bufferIndex-i-1);
            m_bufferIndex-=(i+1);
            break;
        } else if(size-1==i) {
            //dst buffer size will be exceeded
            dest[i]='\0';
            memmove(m_buffer,m_buffer+i,m_bufferIndex-i);
            m_bufferIndex-=i;
            break;
        }
        dest[i]=m_buffer[i];
    }
    if(i==m_bufferIndex) {
        dest[0]='\0';   //we didn't find a new line, returning an empty string
    }
    return strlen(dest);
}

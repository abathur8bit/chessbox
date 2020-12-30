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

#include <string.h>

#include "Connector.h"

#ifdef WIN32
bool Connector::m_bWSAStarted=false;
#endif

Connector::Connector() : m_sock(),m_connected(false),m_bufferIndex(0) {
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

void Connector::connect(const char *host, unsigned short port) {
    m_sock.connect(host,port);
    m_connected=true;
}

void Connector::close() {
    m_sock.close();
    m_connected=false;
}

/**
 * Send a string of data. The number of bytes sent is the length of the null terminated string passed
 * in. Send operation typically doesn't block.
 *
 * @param s String to send.
 * @return The number of bytes that were actually sent.
 */
int Connector::send(const char* s) {
    string buf=s;
    buf+="\r\n";
    return m_sock.send(buf.c_str(),buf.length());
}

int Connector::waitline(char* dest,size_t size) {
    //read any new data
    dest[0]='\0';   //null terminate right away
    char buf[1000];
    int n = m_sock.recv(buf, sizeof(buf));
    if(n>0) {
        int max=n;
        if(m_bufferIndex + n>=CONNECTOR_BUFFER_SIZE) {
            max=CONNECTOR_BUFFER_SIZE - m_bufferIndex;
        }
        if(max) {
            memcpy(m_buffer + m_bufferIndex, buf, max);
            m_bufferIndex+=max;
        }
        parse(dest, size);
    }
    return n;
}
/**
 * Reads data until it gets a full line of data. A full line is ended by a \n char sequence. If less then a full line of data is read, it's buffered.
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
    parse(dest,size);
    return strlen(dest);
}

char* Connector::parse(char* dest,size_t size) {
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
    return dest;
}

void Connector::inspect(bool b) {
    if(b) {
        send("{\"action\":\"setmode\",\"mode\":\"inspect\"}");
    } else {
        send("{\"action\":\"setmode\",\"mode\":\"play\"}");
    }
}
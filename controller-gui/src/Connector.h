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

#ifndef CONTROLLER_GUI_CONNECTOR_H
#define CONTROLLER_GUI_CONNECTOR_H

#ifdef WIN32
#include <winsock.h>
#endif
#include <sys/types.h>
#include <ctype.h>
#ifndef WIN32
# include <netinet/in.h>
# include <netdb.h>
# ifndef USE_POLL
#   include <sys/select.h>    //not using select, using poll instead as select gave me some headaches
# else
#   include <sys/poll.h>
# endif
# include <arpa/inet.h>
# include <sys/time.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <unistd.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#include "socketinstance.h"

using namespace ssobjects;

#define CONNECTOR_BUFFER_SIZE   4000

class Connector {
public:
#ifdef WIN32
    WSADATA             m_wsd;              ///< WSA startup information (Windows only)
    static bool         m_bWSAStarted;      ///< Flag indicating if we have started socket layer.
#endif
    Connector();
    void connect(const char* host,unsigned short port);
    int readline(char* buffer, size_t size);
    int send(const char* s);
    void close();
    bool isConnected() {return m_connected;}

protected:
    SocketInstance m_sock;
    bool m_connected;
    int m_bufferIndex;
    char m_buffer[CONNECTOR_BUFFER_SIZE];
};


#endif //CONTROLLER_GUI_CONNECTOR_H

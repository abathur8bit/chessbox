/********************************************************************
        Copyright (c) 2006, Lee Patterson
        http://ssobjects.sourceforge.net

        Original source from Inside Visual C++

        created  :    5/14/1998
        filename :    socketinstance.cpp
        author   :    Lee Patterson (workerant@users.sourceforge.net)
        
        purpose  :    see header file
*********************************************************************/

//#include "pch.h"

//#ifdef WIN32
//#include "stdafx.h"
//#endif

#include "socketinstance.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>

using namespace ssobjects;

#ifdef __SockAddr__
/** 
    Does a compairison of the sin_family, sin_port and sin_addr to determine if
    the passed socket address is the same. sin_len & sin_zero are not checked as
    sin_len is not always implimented (Linux and Windows) and sin_zero is unused
    and only provided to make socket address structures at least 16-bytes in
    size.

    \return true if they are found to be the same, false otherwise.

    \note sin_zero may have other uses. It may have been used to obtain a byte
    by byte address, but this hasn't been verified.
 **/ 
bool 
SockAddr::operator == (const sockaddr_in& sa)
{
  if(sa.sin_family == m_sa.sin_family &&
     sa.sin_port == m_sa.sin_port &&
     sa.sin_addr.s_addr == m_sa.sin_addr.s_addr)
    return true;

  return false;
}
#endif


#ifdef __SocketInstance__

SocketInstance& 
SocketInstance::operator=(const SocketInstance& s)
{
  if(&s == this)
    return *this;
  m_hSocket = s.m_hSocket;
  m_bThrowExceptionOnReadClose = s.m_bThrowExceptionOnReadClose;
  return *this;
}

void SocketInstance::cleanup()
{
  // doesn't throw an exception because it's called in a catch block
  if(m_hSocket == 0) return;
#   ifdef WIN32
  closesocket(m_hSocket);
#   else
  ::close(m_hSocket);
#   endif
  m_hSocket = 0;
}

void SocketInstance::create(const int nType /* = SOCK_STREAM */)
{
  //  assert(m_hSocket == NULL);
  if((m_hSocket = socket(AF_INET, nType, 0)) == INVALID_SOCKET) {
    throwSocketInstanceException("Create");
  }

  int flag=1;
  if(-1==setsockopt(m_hSocket,SOL_SOCKET,SO_REUSEADDR,(char*)&flag,sizeof(flag)))
    throwSocketInstanceException("Create: Couldn't set the reuse addr option");
}

void SocketInstance::bind(LPCSOCKADDR psa)
{
  //    assert(m_hSocket != NULL);
  if(::bind(m_hSocket, psa, sizeof(SOCKADDR)) == SOCKET_ERROR) {
    throwSocketInstanceException("Bind");
  }
}

void SocketInstance::listen()
{
  //    assert(m_hSocket != NULL);
  if(::listen(m_hSocket, 5) == SOCKET_ERROR) {
    throwSocketInstanceException("Listen");
  }
}

bool SocketInstance::accept(SocketInstance& sConnect, LPSOCKADDR psa)
{
#if 1
  if(!m_hSocket)
    throwSocketInstanceException("Accept: Invalid socket");

  if(sConnect.m_hSocket)
    throwSocketInstanceException("Accept: New socket should not be connected");

  socklen_t iLength=sizeof(SOCKADDR);
  sConnect.m_hSocket = ::accept(m_hSocket,psa,&iLength);
  if(!sConnect.m_hSocket || -1==sConnect.m_hSocket)
  {
    if(!sConnect.m_hSocket)
      throwSocketInstanceException("Accept: Returned zero");
    else
      throwSocketInstanceException("Accept: Returned invalid");
    return false;
  }

  return true;
#else
  //assert(m_hSocket != NULL);
  //assert(sConnect.m_hSocket == NULL);
  int nLengthAddr = sizeof(SOCKADDR);
  sConnect.m_hSocket = ::accept(m_hSocket, psa, &nLengthAddr);
  if(sConnect == INVALID_SOCKET) {
    // no exception if the listen was canceled
    if(errno != WSAEINTR) {
      throwSocketInstanceException("Accept");
    }
    return FALSE;
  }
  return TRUE;
#endif
}

bool SocketInstance::accept(SocketInstance* pConnect, LPSOCKADDR psa)
{
  //    assert(m_hSocket != NULL);
  //    assert(pConnect->m_hSocket == NULL);
  //throwSocketInstanceException("throwing exception for the fun of it");
  socklen_t nLengthAddr = sizeof(SOCKADDR);
  pConnect->m_hSocket = ::accept(m_hSocket, psa, &nLengthAddr);
  if(*pConnect == INVALID_SOCKET) {
#if 1
    throwSocketInstanceException("Accept");
#else
    // no exception if the listen was canceled
    if(WSAGetLastError() != WSAEINTR) {
      throwSocketInstanceException("Accept");
    }
    return FALSE;
#endif
  }
  return TRUE;
}

void SocketInstance::close()
{
  if(m_hSocket==0)
       return;          //calling close on a closed socket should do nothing harmful
    
  //assert(m_hSocket != NULL);
  int er;
  er = 
#   ifdef WIN32
    closesocket(m_hSocket);
#   else
  ::close(m_hSocket);
#   endif
//  if(er == SOCKET_ERROR) {
//    // should be OK to close if closed already
//    throwSocketInstanceException("Close");
//  }
  m_hSocket = 0;
}

void SocketInstance::connect(const char* host,word port)
{
    SockAddr sa(host,port);
    create();
    connect(sa);
}

void SocketInstance::connect(LPCSOCKADDR psa)
{
  //LBP (4/29/99): made so throws an exception instead of asserting when socket is invalid
  if(m_hSocket==0)
    throwSocketInstanceException("Connect - Invalid Socket");
    
  //assert(m_hSocket != NULL);
  // should timeout by itself
  if(::connect(m_hSocket, psa, sizeof(SOCKADDR)) == SOCKET_ERROR) {
    throwSocketInstanceException("Unable to connect");
  }
}

int SocketInstance::write(const char* pch, const int nSize, const int nSecs)
{
  int nBytesSent = 0;
  int nBytesThisTime;
  int nToSend;        //LBP (08/12/99): added to limit packets to 128 bytes , the optimal packet size
  const char* pch1 = pch;
  do {
    //limit packets going out to 128 or less
    //the optimal packet size for sending over
    //over network.
    if(nSize-nBytesSent>128)
      nToSend = 128;
    else
      nToSend = nSize-nBytesSent;

    nBytesThisTime = send(pch1, nToSend, nSecs);
    nBytesSent += nBytesThisTime;
    pch1 += nBytesThisTime;
  } while(nBytesSent < nSize);
  return nBytesSent;
}

int SocketInstance::send(const char* pch, const int nSize, const int nSecs)
{
  //LBP (4/29/99): made send throw exception instead of asserting
  int iFlags=0;
#ifdef OS_LINUX
  iFlags |= MSG_NOSIGNAL;
#endif

  if(m_hSocket==0)
    throwSocketInstanceException("Send - Invalid socket");

  //assert(m_hSocket != 0);
  // returned value will be less than nSize if client cancels the reading
#ifdef USE_POLL
  //try using poll
  int nMilliSecs = nSecs*1000;
  int iReady;
  struct pollfd ufds;

  ufds.fd = m_hSocket;
  ufds.events = POLLOUT;
  
  iReady=poll(&ufds,1,nMilliSecs);
  if(-1==iReady)
    throwSocketInstanceException("Send: Poll error");

  if(!iReady)
    throwSocketInstanceException("Send: Timeout");

#else
//      #ifdef WIN32
//      FD_SET fd = {1, m_hSocket};
//      TIMEVAL tv = {nSecs, 0};
//      TIMEVAL *ptv=NULL;
//      if(nSecs) 
//          ptv=&tv;

//      if(select(0, NULL, &fd, NULL, ptv) == 0) {
//          throwSocketInstanceException("Send timeout");
//      }
//      #endif
  fd_set wset;
  struct timeval tv;
  struct timeval* ptv=NULL;
  int er;
  if(nSecs)
  {
    tv.tv_sec=nSecs;
    tv.tv_usec=0;
    ptv=&tv;
  }
  FD_ZERO(&wset);
  FD_SET(m_hSocket,&wset);
  er = select(m_hSocket+1,NULL,&wset,NULL,ptv);
  if(!er)
    throwSocketInstanceException("timeout");
  else if(-1 == er)
    throwSocketInstanceException("send socket error");
  else
  {
    if(!FD_ISSET(m_hSocket,&wset))
      throwSocketInstanceException("send socket error2");
  }
#endif

  int nBytesSent;
  if((nBytesSent = ::send(m_hSocket, pch, nSize, iFlags)) == SOCKET_ERROR) {
    throwSocketInstanceException("Send");
  }
  return nBytesSent;
}

/**
    When read exceptions are set, all socket errors, and closers will throw an exeption.
    Sometimes however it is desirable not to throw an exception when the connection was
    gracefully closed on the other end. In this case recv will return 0 bytes read.

    \param bThrowException  true causes exceptions will be thrown. 
                            false causes recv to return 0 bytes read.
**/
void SocketInstance::setExceptionOnReadClose(const bool bThrowException)
{ 
  m_bThrowExceptionOnReadClose = bThrowException; 
}

/**
    Read in data from connected socket. If m_bThrowExceptionOnReadClose is set, and 
    the socket is closed gracefully on the other end, an exception is throw. If not set,
    SocketInstance::recv will return 0 bytes.

    \throw SocketInstanceException  If there was an error during the read operation,
    or the socket was closed.
**/
int SocketInstance::recv(char* pch, const int nSize, const int nSecs)
{
  //LBP (4/29/99): made so throws an exception instead of asserting when socket is invalid
  if(m_hSocket==0)
    throwSocketInstanceException("Receive - Invalid Socket");

  //assert(m_hSocket != NULL);
  fd_set fd;
  struct timeval tv = {nSecs, 0};
  struct timeval *ptv = NULL;
  if(nSecs) 
    ptv=&tv;

  FD_ZERO(&fd);
  FD_SET(m_hSocket,&fd);
  int iReady = select(m_hSocket+1, &fd, NULL, NULL, ptv);

  if(!iReady)
    throwSocketInstanceException("Receive timeout");

  if(-1 == iReady)
    throwSocketInstanceException("Receive select error");

  int nBytesReceived = ::recv(m_hSocket, pch, nSize, 0);

  if(SOCKET_ERROR == nBytesReceived) 
    throwSocketInstanceException("Receive");

  if(!nBytesReceived && m_bThrowExceptionOnReadClose)
    throwSocketInstanceException("Connection closed");

  return nBytesReceived;
}

int SocketInstance::receiveDatagram(char* pch, const int nSize, LPSOCKADDR psa, const int nSecs)
{
  //LBP (4/29/99): made so throws an exception instead of asserting when socket is invalid
  if(m_hSocket==0)
    throwSocketInstanceException("ReceiveDatagram - Invalid Socket");
    
  //assert(m_hSocket != NULL);
  fd_set rset;
  struct timeval tv;
  struct timeval *ptv=NULL;

  if(nSecs)
  {
    tv.tv_sec = nSecs;
    tv.tv_usec = 0;
    ptv = &tv;
  }

  FD_ZERO(&rset);
  FD_SET(m_hSocket,&rset);

  int iReady = select(m_hSocket+1,&rset,NULL,NULL,ptv);
  if(!iReady)
    throwSocketInstanceException("Receive datagram timeout");

  // input buffer should be big enough for the entire datagram
  socklen_t nFromSize = sizeof(SOCKADDR);
  int nBytesReceived = ::recvfrom(m_hSocket, pch, nSize, 0, psa, &nFromSize);
  if(nBytesReceived == SOCKET_ERROR) {
    throwSocketInstanceException("ReceiveDatagram");
  }
  return nBytesReceived;
}

int SocketInstance::sendDatagram(const char* pch, const int nSize, LPCSOCKADDR psa, const int nSecs)
{
  UNUSED_ALWAYS(nSecs);

  //LBP (4/29/99): made so throws an exception instead of asserting when socket is invalid
  if(m_hSocket==0)
    throwSocketInstanceException("SendDatagram - Invalid Socket");
    
  //assert(m_hSocket != NULL);
// not selecting as a udp send usually doesn't fail
//    #ifdef WIN32
//    FD_SET fd = {1, m_hSocket};
//    TIMEVAL tv = {nSecs, 0};
//    if(select(0, NULL, &fd, NULL, &tv) == 0) {
//        throwSocketInstanceException("Send timeout");
//    }
//    #endif

  int nBytesSent = ::sendto(m_hSocket, pch, nSize, 0, psa, sizeof(SOCKADDR));
  if(nBytesSent == SOCKET_ERROR) {
    throwSocketInstanceException("SendDatagram");
  }
  return nBytesSent;
}

void SocketInstance::getPeerAddr(LPSOCKADDR psa)
{
  //LBP (4/29/99): made so throws an exception instead of asserting when socket is invalid
  if(m_hSocket==0)
    throwSocketInstanceException("GetPeerAddr - Invalid Socket");
    
  //assert(m_hSocket != NULL);
  // gets the address of the socket at the other end
  socklen_t nLengthAddr = sizeof(SOCKADDR);
  if(getpeername(m_hSocket, psa, &nLengthAddr) == SOCKET_ERROR) {
    throwSocketInstanceException("GetPeerName");
  }
}

void SocketInstance::getSockAddr(LPSOCKADDR psa)
{
  //LBP (4/29/99): made so throws an exception instead of asserting when socket is invalid
  if(m_hSocket==0)
    throwSocketInstanceException("GetSockAddr - Invalid Socket");
    
  //assert(m_hSocket != NULL);
  // gets the address of the socket at this end
  socklen_t nLengthAddr = sizeof(SOCKADDR);
  if(getsockname(m_hSocket, psa, &nLengthAddr) == SOCKET_ERROR) {
    throwSocketInstanceException("GetSockName");
  }
}

//static
SockAddr SocketInstance::getHostByName(const char* pchName, const USHORT ushPort /* = 0 */)
{
  struct hostent* pHostEnt = gethostbyname(pchName);
  if(pHostEnt == NULL) {
    throwSocketInstanceException("GetHostByName");
  }
  ULONG* pulAddr = (ULONG*) pHostEnt->h_addr_list[0];
  SOCKADDR_IN sockTemp;
  sockTemp.sin_family = AF_INET;
  sockTemp.sin_port = htons(ushPort);
  sockTemp.sin_addr.s_addr = *pulAddr; // address is already in network byte order
  return sockTemp;
}

//static
const char* SocketInstance::getHostByAddr(LPCSOCKADDR psa)
{
  hostent* pHostEnt = gethostbyaddr((char*) &((LPSOCKADDR_IN) psa)
                                    ->sin_addr.s_addr, 4, PF_INET);
  if(pHostEnt == NULL) {
    throwSocketInstanceException("GetHostByAddr");
  }
  return pHostEnt->h_name; // caller shouldn't delete this memory
}
#endif

//////////////////////////////////////////
//  Class TelnetSocket 
#ifdef __TelnetSocket__
TelnetSocket::TelnetSocket()
  : m_pLoginName(NULL),m_wFlags(0),m_pReadBuf(NULL),m_nReadBuf(0)
{
  m_pReadBuf   = new char[nSizeRecv];
}

TelnetSocket::TelnetSocket(SocketInstance& s)
  : m_pLoginName(NULL),m_wFlags(0),m_pReadBuf(NULL),m_nReadBuf(0)
{
  m_hSocket = s.m_hSocket;
}

TelnetSocket::TelnetSocket(SocketInstance* s)
        : m_pLoginName(NULL),m_wFlags(0),m_pReadBuf(NULL),m_nReadBuf(0)
{
  m_hSocket = s->m_hSocket;
}

TelnetSocket::~TelnetSocket()
{
  delete [] m_pReadBuf;
  if(m_pLoginName) delete [] m_pLoginName;
}

bool TelnetSocket::setLoginName(const char* pname)
{
  m_pLoginName=(char*)malloc(strlen(pname)+1);
  if(!m_pLoginName)
    return false;

  strcpy(m_pLoginName,pname);
  m_pLoginName[strlen(pname)]='\0';
  return true;
}

int TelnetSocket::readLine(char* pch, const int nSize, const int nSecs)
// reads an entire header line through CRLF (or socket close)
// inserts zero string terminator, object maintains a buffer
{
  int nBytesThisTime = m_nReadBuf;
  int nLineLength = 0;
  char* pch1 = m_pReadBuf;
  char* pch2;
  do {
    // look for lf (assume preceded by cr)
    if((pch2 = (char*) memchr(pch1 , '\n', nBytesThisTime)) != NULL) {
      assert((pch2) > m_pReadBuf);
      assert(*(pch2 - 1) == '\r');
      nLineLength = (pch2 - m_pReadBuf) + 1;
      if(nLineLength >= nSize) nLineLength = nSize - 1;
      memcpy(pch, m_pReadBuf, nLineLength); // copy the line to caller
      m_nReadBuf -= nLineLength;
      memmove(m_pReadBuf, pch2 + 1, m_nReadBuf); // shift remaining characters left
      break;
    }
    pch1 += nBytesThisTime;
    nBytesThisTime = recv(m_pReadBuf + m_nReadBuf, nSizeRecv - m_nReadBuf, nSecs);
    if(nBytesThisTime <= 0) { // sender closed socket or line longer than buffer
      throwSocketInstanceException("ReadHeaderLine");
    }
        
    //update telnet session
    try 
    {
      write(m_pReadBuf + m_nReadBuf,nBytesThisTime);
    }
    catch(SocketInstanceException& e) 
    {
      (void)e;
      throwSocketInstanceException("ReadHeaderLine::write");
    }
        
    m_nReadBuf += nBytesThisTime;
  }
  while(TRUE);
  *(pch + nLineLength-2) = '\0';  //add rest of buffer, and remove lfcr
  return nLineLength;
}

int TelnetSocket::readResponse(char* pch, const int nSize, const int nSecs)
// reads remainder of a transmission through buffer full or socket close
// (assume headers have been read already)
{
  int nBytesToRead, nBytesThisTime, nBytesRead = 0;
  if(m_nReadBuf > 0) { // copy anything already in the recv buffer
    memcpy(pch, m_pReadBuf, m_nReadBuf);
    pch += m_nReadBuf;
    nBytesRead = m_nReadBuf;
    m_nReadBuf = 0;
  }
  do { // now pass the rest of the data directly to the caller
    nBytesToRead = min(nSizeRecv, nSize - nBytesRead);
    nBytesThisTime = recv(pch, nBytesToRead, nSecs);
    if(nBytesThisTime <= 0) break; // sender closed the socket
    pch += nBytesThisTime;
    nBytesRead += nBytesThisTime;
  }
  while(nBytesRead <= nSize);
  return nBytesRead;
}

//writes a line of text to the socket, and addes CR & LF
int TelnetSocket::println(const char* fmt,...)
{
  char buffer[1024];
  va_list marker;

  va_start(marker,fmt);
  vsprintf(buffer,fmt,marker);

  int iBytes;
  strcat(buffer,"\r\n");
  iBytes=write(buffer,strlen(buffer));
  return iBytes;
}

//writes a line of text to socket
int TelnetSocket::print(const char* fmt,...)
{
  char buffer[1024];
  va_list marker;

  va_start(marker,fmt);
  vsprintf(buffer,fmt,marker);

  int iBytes;
  iBytes=write(buffer,strlen(buffer));
  return iBytes;
}

#endif

#ifdef __HttpBlockingSocket__
HttpBlockingSocket::HttpBlockingSocket()
  : m_pReadBuf(NULL),m_nReadBuf(0)
{
  m_pReadBuf = new char[nSizeRecv];
}

HttpBlockingSocket::~HttpBlockingSocket()
{
  delete [] m_pReadBuf;
}

int HttpBlockingSocket::readHttpHeaderLine(char* pch, const int nSize, const int nSecs)
// reads an entire header line through CRLF (or socket close)
// inserts zero string terminator, object maintains a buffer
{
  int nBytesThisTime = m_nReadBuf;
  int nLineLength = 0;
  char* pch1 = m_pReadBuf;
  char* pch2;
  do {
    // look for lf (assume preceded by cr)
    if((pch2 = (char*) memchr(pch1 , '\n', nBytesThisTime)) != NULL) {
      assert((pch2) > m_pReadBuf);
      assert(*(pch2 - 1) == '\r');
      nLineLength = (pch2 - m_pReadBuf) + 1;
      if(nLineLength >= nSize) nLineLength = nSize - 1;
      memcpy(pch, m_pReadBuf, nLineLength); // copy the line to caller
      m_nReadBuf -= nLineLength;
      memmove(m_pReadBuf, pch2 + 1, m_nReadBuf); // shift remaining characters left
      break;
    }
    pch1 += nBytesThisTime;
    nBytesThisTime = recv(m_pReadBuf + m_nReadBuf, nSizeRecv - m_nReadBuf, nSecs);
    if(nBytesThisTime <= 0) { // sender closed socket or line longer than buffer
      throwSocketInstanceException("ReadHeaderLine");
    }
    m_nReadBuf += nBytesThisTime;
  }
  while(TRUE);
  *(pch + nLineLength) = '\0';
  return nLineLength;
}

int HttpBlockingSocket::readHttpResponse(char* pch, const int nSize, const int nSecs)
// reads remainder of a transmission through buffer full or socket close
// (assume headers have been read already)
{
  int nBytesToRead, nBytesThisTime, nBytesRead = 0;
  if(m_nReadBuf > 0) { // copy anything already in the recv buffer
    memcpy(pch, m_pReadBuf, m_nReadBuf);
    pch += m_nReadBuf;
    nBytesRead = m_nReadBuf;
    m_nReadBuf = 0;
  }
  do { // now pass the rest of the data directly to the caller
    nBytesToRead = min(nSizeRecv, nSize - nBytesRead);
    nBytesThisTime = recv(pch, nBytesToRead, nSecs);
    if(nBytesThisTime <= 0) break; // sender closed the socket
    pch += nBytesThisTime;
    nBytesRead += nBytesThisTime;
  }
  while(nBytesRead <= nSize);
  return nBytesRead;
}
#endif

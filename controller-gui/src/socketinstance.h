/********************************************************************
        Copyright (c) 2006, Lee Patterson
        http://ssobjects.sourceforge.net

        Original source from Inside Visual C++

        created  :  5/14/1998
        filename :  socketinstance.h
        author   :  Lee Patterson (workerant@users.sourceforge.net)
        
        purpose  :  Base socket class. Also http and telnet 
                    classes. Defaults to blocking mode.

        notes    :  Typical client session would look like this:
                    
                    ...
                    SocketInstance sConnection;
                    try
                    {
                        //open connection
                        puts("Connecting to server...");
                        SockAddr saServer(szHost,iPort);
                        if(isalpha(szHost[0]))  //check if szHost is in "host.com" format or "255.255.255.255"
                            saServer = SocketInstance::getHostByName(szHost,iPort);
                        sConnection.create();
                        sConnection.connect(saServer);
                        //we are connected, so some transfering
                        ...
                        sConnection.close();
                    }
                    catch(GeneralException* e)
                    {
                        sConnection.cleanup();
                        LOG("error %s",e->getErrorMsg());
                    }
                    ...
                    
                    A server would look like this:
                    
                    ...
                    SocketInstance sClient;
                    SockAddr saClient;
                    SockAddr saServer(INADDR_ANY,atoi(argv[1]));
                    SocketInstance sListen;
                    try
                    {
                        sListen.create();
                        sListen.bind(saServer);
                        sListen.listen();
                        while(bWantMoreConnections)
                        {
                            sListen.accept(sClient,saClient);
                            //we got a connection ...
                            sClient.close();
                        }
                        sListen.close();
                    }
                    catch(GeneralException* e)
                    {
                        sConnection.cleanup();
                        LOG("error %s",e->getErrorMsg());
                    }
                    ...
                    
                    

*********************************************************************/

#ifndef SOCKETINSTANCE_H
#define SOCKETINSTANCE_H

#ifdef OS_LINUX
#define USE_POLL
#endif
//#pragma warning(push)
//#pragma warning(disable: )
#define __SockAddr__
#define __SocketInstance__
#define __SocketInstanceException__
#define __TelnetSocket__
#define __HttpBlockingSocket__

#ifdef WIN32
#include <winsock.h>
#endif

#include <sys/types.h>
#include <ctype.h>
//#include <stdio.h>

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
#include "defs.h"
#include "generalexception.h"
#include "errno.h"

enum {
    DEFAULT_SOCKET_TIMEOUT=600,     //10 minutes
    NO_TIMEOUT=0,                   //never time out    
};

#define TELNET_PORT         23
#define WEB_PORT            80

#ifndef WIN32
# define WSAEWOULDBLOCK       EWOULDBLOCK
# define SOCKET_ERROR        (-1)
# define INVALID_SOCKET      (-1)
# define closesocket(s)      close(s)
# define WSAGetLastError()   errno

typedef int SOCKET;
typedef struct sockaddr        SOCKADDR;

typedef struct sockaddr_in     SOCKADDR_IN;
typedef struct sockaddr_in*    LPSOCKADDR_IN;
#endif

#ifdef WIN32
  typedef int socklen_t;
#endif

typedef struct timeval  TIMEVAL;
typedef unsigned long   ULONG;
typedef unsigned short  USHORT;

typedef const struct sockaddr* LPCSOCKADDR;
typedef struct sockaddr*       LPSOCKADDR;

namespace ssobjects
{



// member functions truly block and must not be used in UI threads
// use this class as an alternative to the MFC CSocket class

#ifdef __SocketInstanceException__
#define throwSocketInstanceException(m) (throw SocketInstanceException(m,__FILE__,__LINE__))

/**
   \brief
   Socket error. 

   Socket operations that fail throw this type.
**/
class SocketInstanceException : public GeneralException
{
public:
    SocketInstanceException(const char* pchMessage,const char* pFname,const int iLine) : GeneralException(pchMessage,pFname,iLine){}
    ~SocketInstanceException(){}
};
#endif

#ifdef __SocketInstance__
class SockAddr;

/**
   \brief
   The socket that you use to connect with.

   The SocketInstance object is the lowest level in the socket layer.
   It provides access methods to read and write to a socket un TCP/IP 
   or UDP mode.

   Typical client session would look like this:

   <PRE>
   ...
   SocketInstance sConnection;
   try
   {
     //open connection
     puts("Connecting to server...");
     SockAddr saServer(szHost,iPort);
     if(isalpha(szHost[0]))  //check if szHost is in "host.com" format or "255.255.255.255"
       saServer = SocketInstance::getHostByName(szHost,iPort);
     sConnection.create();
     sConnection.connect(saServer);
     //we are connected, do something useful
     ...
     sConnection.close();
   }
   catch(GeneralException* e)
   {
     sConnection.cleanup();
     LOG("error %s",e->getErrorMsg());
   }
   ...
   <P>
   A server would look like this:
   <P>                 
   ...
   SocketInstance sClient;
   SockAddr saClient;
   SockAddr saServer(INADDR_ANY,atoi(argv[1]));
   SocketInstance sListen;
   try
   {
     sListen.create();
     sListen.bind(saServer);
     sListen.listen();
     while(bWantMoreConnections)
     {
       sListen.accept(sClient,saClient);
       //we got a connection ...
       sClient.close();
     }
     sListen.close();
   }
   catch(GeneralException* e)
   {
     sConnection.cleanup();
     LOG("error %s",e->getErrorMsg());
   }
   ...
   </PRE>                    

**/
class SocketInstance 
{
  public:
    SOCKET m_hSocket;
    bool   m_bThrowExceptionOnReadClose;
    SocketInstance() : m_hSocket(0),m_bThrowExceptionOnReadClose(true) {}
    virtual ~SocketInstance(){}
    SOCKET getSocket() const {return m_hSocket;}
    void setExceptionOnReadClose(const bool bThrowException=true);  ///< Toggles exceptions on read closers. 
    void cleanup();
    void create(const int nType = SOCK_STREAM);
    void close();
    void bind(LPCSOCKADDR psa);
    void listen();
    void connect(const char* host,unsigned short port);
    void connect(LPCSOCKADDR psa);
    bool accept(SocketInstance& s, LPSOCKADDR psa);
    bool accept(SocketInstance* s, LPSOCKADDR psa);
    int send(const char* pch, const int nSize, const int nSecs=DEFAULT_SOCKET_TIMEOUT);
    int write(const char* pch, const int nSize, const int nSecs=DEFAULT_SOCKET_TIMEOUT);
    int recv(char* pch, const int nSize, const int nSecs=DEFAULT_SOCKET_TIMEOUT);
    int sendDatagram(const char* pch, const int nSize, LPCSOCKADDR psa, 
                     const int nSecs=10);
    int receiveDatagram(char* pch, const int nSize, LPSOCKADDR psa, 
                        const int nSecs=10);
    void getPeerAddr(LPSOCKADDR psa);
    void getSockAddr(LPSOCKADDR psa);
    static SockAddr getHostByName(const char* pchName, 
                                    const USHORT ushPort = 0);
    static const char* getHostByAddr(LPCSOCKADDR psa);
    operator SOCKET()
    { return m_hSocket; }
    
    SocketInstance& operator=(const SocketInstance& s);
    SocketInstance(const SocketInstance& s) : m_hSocket(s.m_hSocket),m_bThrowExceptionOnReadClose(s.m_bThrowExceptionOnReadClose) {}
};
#endif

#ifdef __SockAddr__
/**
   \brief
   Replacement for sockaddr_in structure.

   Provides a much easier way to access and manipulate a socket address structure.
**/
class SockAddr
{
  private:
    struct sockaddr_in m_sa;
    
  public:
    // constructors
    SockAddr() : m_sa()
    { 
      	memset(&m_sa,0,sizeof(m_sa)); 
      	m_sa.sin_family = AF_INET; 
      	m_sa.sin_port = 0; 
      	m_sa.sin_addr.s_addr = 0; 
    } // Default
    SockAddr(const sockaddr& sa) : m_sa() { memcpy(&m_sa, &sa, sizeof(SOCKADDR)); }
    SockAddr(const sockaddr_in& sin) : m_sa() { memcpy(&m_sa, &sin, sizeof(SOCKADDR_IN)); }
    SockAddr(const ULONG ulAddr, const USHORT ushPort = 0) : m_sa() // parms are host byte ordered
    { 
    	memset(&m_sa,0,sizeof(sockaddr_in));
        m_sa.sin_family = AF_INET;
        m_sa.sin_port = htons(ushPort);
        m_sa.sin_addr.s_addr = htonl(ulAddr); 
        
    }
#ifdef OS_LINUX
    SockAddr(const uint32_t ulAddr, const USHORT ushPort = 0) : m_sa() // parms are host byte ordered
    { 
    	m_sa.sin_family = AF_INET;
        m_sa.sin_port = htons(ushPort);
        m_sa.sin_addr.s_addr = htonl(ulAddr); 
    }
#endif
    SockAddr(const char* pchIP, const USHORT ushPort = 0) : m_sa()// dotted IP addr string
    { 
      if(isalpha(pchIP[0])) //check if IP is in "host.com" format or "255.255.255.255"
      {
        struct hostent* pHostEnt = gethostbyname(pchIP);
        if(pHostEnt == NULL) {
//          CStr s;
//          int err = WSAGetLastError();
//          strerror(err);
//          s.format("SockAddr GetHostByName error %d:%s",err,strerror(err));
//          throwSocketInstanceException((const char*)s);
            throwSocketInstanceException("GetHostByName error");
        }
        ULONG* pulAddr = (ULONG*) pHostEnt->h_addr_list[0];
        m_sa.sin_family = AF_INET;
        m_sa.sin_port = htons(ushPort);
        m_sa.sin_addr.s_addr = *pulAddr; // address is already in network byte order
                
      }
      else
      {
        m_sa.sin_family = AF_INET;
        m_sa.sin_port = htons(ushPort);
        m_sa.sin_addr.s_addr = inet_addr(pchIP); 
      }
    } // already network byte ordered
    
    // Return the address in dotted-decimal format
    char* dottedDecimal()
    { return inet_ntoa(m_sa.sin_addr); }
    // Get port and address (even though they're public)
    USHORT port() const { return ntohs(m_sa.sin_port); }
    ULONG ipAddr() const { return ntohl(m_sa.sin_addr.s_addr); }
    // operators added for efficiency
    const SockAddr& operator=(const SOCKADDR& sa)
    { 
        memcpy(&m_sa, &sa, sizeof(SOCKADDR));
        return *this;
    }
    const SockAddr& operator=(const SOCKADDR_IN& sin)
    {
      memcpy(&m_sa, &sin, sizeof(SOCKADDR_IN));
      return *this; 
    }
    operator SOCKADDR()
    { 
      return *((LPSOCKADDR)&m_sa); 
    }
    operator LPSOCKADDR() { return (LPSOCKADDR) &m_sa; }
    operator LPSOCKADDR_IN() { return (LPSOCKADDR_IN) &m_sa; }
    bool operator == (const sockaddr_in& sa);     ///< Compairs two socket addresses.
};
#endif


#ifdef __TelnetSocket__
class TelnetSocket : public SocketInstance
{
  public:
    enum {nSizeRecv = 1024}; // max receive buffer size (> hdr line length)
    enum {FLAG_VALIDATED=1,FLAG_PLAYING=2};
    TelnetSocket();
    TelnetSocket(SocketInstance& s);
    TelnetSocket(SocketInstance* s);
    virtual ~TelnetSocket();
    int readLine(char* pch, const int nSize, const int nSecs=DEFAULT_SOCKET_TIMEOUT);
    int readResponse(char* pch, const int nSize, const int nSecs=DEFAULT_SOCKET_TIMEOUT);
    int println(const char* fmt,...);
    int print(const char* fmt,...);
    bool setLoginName(const char* name);

  public:
    //login information
    char* m_pLoginName;
    unsigned short m_wFlags;

  private:
    char* m_pReadBuf; // read buffer
    int m_nReadBuf; // number of bytes in the read buffer

  private:
    TelnetSocket(const TelnetSocket&);
    TelnetSocket& operator=(const TelnetSocket&);
};
#endif

#ifdef __HttpBlockingSocket__
class HttpBlockingSocket : public SocketInstance
{
  public:
    enum {nSizeRecv = 1000}; // max receive buffer size (> hdr line length)
    HttpBlockingSocket();
    ~HttpBlockingSocket();
    int readHttpHeaderLine(char* pch, const int nSize, const int nSecs);
    int readHttpResponse(char* pch, const int nSize, const int nSecs);
  private:
    char* m_pReadBuf; // read buffer
    int m_nReadBuf; // number of bytes in the read buffer
  private:
    //unused overloads
    HttpBlockingSocket(const HttpBlockingSocket&);
    HttpBlockingSocket& operator=(const HttpBlockingSocket&);
};
#endif

};
//#pragma warning(pop)

#endif


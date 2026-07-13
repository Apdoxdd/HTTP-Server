#ifndef HTTP_SERVER
#define HTTP_SERVER

#include "httpRequest.hpp"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <mswsock.h>

class httpServer 
{
private:
    std::string contentPath;
    SOCKET server = INVALID_SOCKET;
public:
    bool init( int port = 8080 );
    void acceptAndServe();
    void getRequest( char* recBuf, httpRequest& msg, int &bytesRec );
    void startup ( int port = 8080);
    httpServer ();
    
};



#endif

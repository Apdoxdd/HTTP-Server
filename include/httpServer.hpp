#ifndef HTTP_SERVER
#define HTTP_SERVER

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <mswsock.h>

#include "httpRequest.hpp"
#include "threadPool.hpp"

class httpServer 
{
private:
    threadPool thPool;
    std::string contentPath;
    SOCKET server = INVALID_SOCKET;
    SOCKET serverTls = INVALID_SOCKET;
    SSL_CTX *context;
    
public:
    bool init( int portHTTP = 8080, int portHTTPS = 4040 );
    void acceptAndServe();
    void getRequest( const char* recBuf, httpRequest& msg, int &bytesRec );
    void startup ( int port = 8080, int tlsPort = 4040);
    bool validateRequest( httpRequest& msg, SOCKET& client ); 
    void serveRequest( SOCKET client, SSL *con );
    SSL_CTX *createContext( const std::string &certFile, const std::string &keyFile );
    void acceptAndServeTls();
    httpServer ();
    
};



#endif

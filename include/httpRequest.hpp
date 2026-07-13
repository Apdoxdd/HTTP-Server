#ifndef HTTP_REQUEST
#define HTTP_REQUEST
#include <string>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <mswsock.h>

class httpRequest 
{
private:
    std::string method;
    std::string url;
    std::string version;
    std::string headers;
    std::string body;
    std::string connection;
    std::string contLength;
public:
    httpRequest ();
    void extractMethod( std::string& msg );
    void extractURL( std::string& msg );
    void extractVersion( std::string& msg );
    void extractHeaders( std::string& msg );
    void extractBody( std::string& msg );





friend class httpServer;


friend void HTTP_ERROR( int code, SOCKET& client, std::string& path );
friend void HTTP_GET( httpRequest &msg, SOCKET& client, std::string& path );
friend void HTTP_DELETE( httpRequest &msg, SOCKET& client, std::string& path );
friend void HTTP_PUT( httpRequest &msg, SOCKET& client, std::string& path );


friend void headerConnection ( std::string &value, httpRequest &msg );
friend void headerLength( std::string &value, httpRequest &msg );


};







#endif

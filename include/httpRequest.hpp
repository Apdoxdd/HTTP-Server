#ifndef HTTP_REQUEST
#define HTTP_REQUEST
#include <string>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <mswsock.h>


 
//#include <openssl/ssl.h>
//#include <openssl/err.h>

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
    std::string host;
    std::string encoding;
    std::string expect;

public:
    httpRequest ();
    size_t extractMethod( std::string& msg, size_t lead = 0);
    size_t extractURL( std::string& msg, size_t lead );
    size_t extractVersion( std::string& msg, size_t lead );
    size_t extractHeaders( std::string& msg, size_t lead );
    size_t extractBody( std::string& msg, size_t lead);





friend class httpServer;


friend void HTTP_ERROR( int code, SOCKET& client );
friend void HTTP_GET( httpRequest &msg, SOCKET& client, std::string& path );
friend void HTTP_DELETE( httpRequest &msg, SOCKET& client, std::string& path );
friend void HTTP_PUT( httpRequest &msg, SOCKET& client, std::string& path );
friend void HTTP_HEAD( httpRequest &msg, SOCKET& client, std::string& path );
friend void HTTP_POST( httpRequest &msg, SOCKET& client, std::string& path );


friend void headerConnection ( std::string &value, httpRequest &msg );
friend void headerLength( std::string &value, httpRequest &msg );
friend void headerHost ( std::string &value, httpRequest &msg );
friend void headerEncoding( std::string &value, httpRequest &msg);

friend void headerExpect( std::string &value, httpRequest &msg);
};







#endif

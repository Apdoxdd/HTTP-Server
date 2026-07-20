#ifndef METHODS
#define METHODS

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <mswsock.h>
#include "httpRequest.hpp"

// methods the server supports
void HTTP_ERROR( int code, SOCKET& client );
void HTTP_GET( httpRequest &msg, SOCKET& client, std::string& path );
void HTTP_DELETE( httpRequest &msg, SOCKET& client, std::string& path );
void HTTP_PUT( httpRequest &msg, SOCKET& client, std::string& path );
void HTTP_HEAD( httpRequest &msg, SOCKET& client, std::string& path );

// headers the server supports
void parseHeaders ( std::string &headers, httpRequest &msg );
void headerConnection ( std::string &value, httpRequest &msg);
void headerLength( std::string &value, httpRequest &msg );
void headerHost( std::string &value, httpRequest &msg );
void headerEncoding( std::string &value, httpRequest &msg);



// server helper functions
std::string getDateNdTime();

#endif

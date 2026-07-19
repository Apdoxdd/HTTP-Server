#include <WinSock2.h>
#include <WS2tcpip.h>
#include <algorithm>
#include <mswsock.h>
#include "../include/httpRequest.hpp"
#include "../include/maps.hpp"
#include "methods.hpp"
#include "../include/httpServer.hpp"
#include <errhandlingapi.h>
#include <string>
#include <iostream>
#include <cctype>
#include <algorithm>
#include <thread>
#include <filesystem>
#include <windows.h>


httpServer::httpServer()
{
    char buffer[ MAX_PATH ];
    GetModuleFileNameA( NULL, buffer, MAX_PATH );

    std::filesystem::path exeDir = std::filesystem::path(buffer).parent_path();
    contentPath = (exeDir  / "../content/").lexically_normal().string();

}







bool httpServer::init ( int port )
{
    WSAData temp;
    int res = WSAStartup( MAKEWORD( 2, 2 ), &temp );
    if ( res != 0 )
    {
        std::cout<<"WSAStartup failed"<<std::endl;
        return 0;
    }

    server = socket ( AF_INET, SOCK_STREAM, IPPROTO_TCP );
    if ( server == INVALID_SOCKET )
    {
        std::cout<<"Error at socket(), code: "<<WSAGetLastError()<<std::endl;
        WSACleanup();
        return 0;
    }

    sockaddr_in serverAD {};
    serverAD.sin_family = AF_INET;
    serverAD.sin_port   = htons( port );
    serverAD.sin_addr.s_addr = INADDR_ANY;

    res = bind ( server, (SOCKADDR*) &serverAD, sizeof( serverAD ) );
    if ( res == SOCKET_ERROR )
    {
        std::cout<<"Error at bind(), code: "<<WSAGetLastError()<<std::endl;
        closesocket ( server );
        WSACleanup();
        return 0;
    }

    res = listen ( server, SOMAXCONN );
    if ( res == SOCKET_ERROR )
    {
        std::cout<<"Error at listen(), code: "<<WSAGetLastError()<<std::endl;
        closesocket( server );
        WSACleanup();
        return 0;
    }
    return true;




}
void httpServer::getRequest ( const char* recBuf, httpRequest &msg, int &bytesRec )
{
    std::string req ( recBuf, bytesRec );
    msg.extractMethod( req );
    msg.extractURL( req );
    msg.extractVersion( req );
    msg.extractHeaders( req );
    std::transform( msg.headers.begin(), msg.headers.end(), msg.headers.begin(), [](unsigned char c)
            {return std::tolower (c);});
    parseHeaders(msg.headers, msg );
    msg.extractBody( req );
    if ( msg.version == "HTTP/1.0" )
        msg.connection = "close";

}

void httpServer::acceptAndServe ()
{
    while ( true )
    {
        SOCKET client = accept ( server, NULL, NULL );
        if ( client == INVALID_SOCKET )
            continue;
        // place holder for when i spwan a new thread
        std::thread worker ( [=]() mutable {


        while ( true )
        {

            httpRequest msg {};
            std::string accumlate{};
            int bytes {0};
            while ( accumlate.find("\r\n\r\n") == std::string::npos && client != INVALID_SOCKET)
            {        
                char recvBuf [4097] {};
                int bytesRec = recv( client, recvBuf, 4096, 0 );
                if ( bytesRec <= 0) 
                {
                    std::cout<<"Error overflow in the recived request"<<std::endl;
                    closesocket(client);
                    client = INVALID_SOCKET;
                    break;
                }
                if ( accumlate.size() > 125000 )
                {
                    HTTP_ERROR(400, client);
                    closesocket(client);
                    client = INVALID_SOCKET;
                    break;
                }
                recvBuf[ bytesRec ] = '\0';
                std::string temp ( recvBuf, bytesRec );
                accumlate += temp;
                bytes += bytesRec;
                
            }
            if ( client == INVALID_SOCKET )
                
            {
                break;
            }
            const char* recvBuf = accumlate.c_str();
                
            getRequest( recvBuf, msg, bytes );

            if ( validateRequest( msg, client) ) 
            {
                methodMap.at( msg.method ) ( msg, client, contentPath );
            }
            if ( msg.connection == "close" )
            {
                closesocket( client );
                break;
            }
        }


                });
        worker.detach();
    }
    closesocket( server );


}

bool httpServer::validateRequest( httpRequest& msg, SOCKET& client )
{
    
            auto methodCheck = methodMap.find ( msg.method );

            if ( msg.version != "HTTP/1.1" && msg.version != "HTTP/1.0" )
            {
                if ( msg.version == "HTTP/2" || msg.version == "HTTP/3" )
                    HTTP_ERROR(505, client);
                else
                 HTTP_ERROR( 400, client );
                msg.connection = "close";
                return false;

            }
            else if ( msg.host == "none" || msg.host == "" )
            {
                HTTP_ERROR ( 400, client );
                msg.connection = "close";
                return false;
            }
            else if ( methodCheck == methodMap.end () )
            {
                HTTP_ERROR( 405, client );
                msg.connection = "close";
                return false;
            }
            else if ( msg.encoding != "whole" )
            {
                if ( msg.encoding == "chunked" )
                    HTTP_ERROR( 501, client );
                else
                 HTTP_ERROR( 400, client );
                msg.connection = "close";
                return false;

            }
            else if ( msg.contLength == "err" )
            {
                HTTP_ERROR( 400, client );
                msg.connection = "close";
                return false;
            }
            return true;
}

void httpServer::startup( int port )
{
    //run init, if true run accept and serve()
    if ( !init( port ) )
    {
        std::cout<<"Error initializing the server"<<std::endl;
        return ;
    }
    acceptAndServe();



}


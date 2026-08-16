#include <WinSock2.h>
#include <WS2tcpip.h>
#include <mswsock.h>
#include "../include/httpRequest.hpp"
#include "../include/maps.hpp"
#include "methods.hpp"
#include "../include/httpServer.hpp"
#include "../include/logger.hpp"
#include "../include/maps.hpp"
#include <errhandlingapi.h>
#include <string>
#include <iostream>
#include <thread>
#include <filesystem>
#include <windows.h>
#include <chrono>
#include <thread>

httpServer::httpServer()
{
    char buffer[ MAX_PATH ];
    GetModuleFileNameA( NULL, buffer, MAX_PATH );

    std::filesystem::path exeDir = std::filesystem::path(buffer).parent_path();
    contentPath = (exeDir  / "../content/").lexically_normal().string();
    // exeDir / string is an overload of the '/' operator in the filesystem object that
    // allows adding names to the filesystem object without complicated functions
    // lexically normal removes dots in ../../build for example
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

    res = listen ( server, -65'536 ); // since SOMAXCONN alone was causing alot of refused requests 
                                    // the number is negative since the macro SOMA is just inverse of the 
                                    // number used, so i put it as negative to bypass the need to include 
                                    // a new header
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
    size_t i {0};
    std::string req ( recBuf, bytesRec );
    i = msg.extractMethod( req, i );
    if ( i == std::string::npos )
        return;
    i = msg.extractURL( req, i );
    if ( i == std::string::npos )
        return;
    i = msg.extractVersion( req, i );
    if ( i == std::string::npos )
        return;
    i = msg.extractHeaders( req, i );
    if ( i == std::string::npos )
        return;

    parseHeaders(msg.headers, msg );
    i = msg.extractBody( req, i );
    if ( msg.version == "HTTP/1.0" )
        msg.connection = "close";

}

void httpServer::serveRequest( SOCKET client )
{
    auto timeOut = std::chrono::high_resolution_clock::now();
        while ( true )
        {

            httpRequest msg {};
            std::string accumlate{};
            int bytes {0};
            size_t searchStart {0};
            auto start = std::chrono::high_resolution_clock::now();
            
            while ( accumlate.find("\r\n\r\n", searchStart) == std::string::npos && client != INVALID_SOCKET)
            {       
                char recvBuf [4097] {};
                int bytesRec = recv( client, recvBuf, 4096, 0 );
                
                if ( bytesRec == 0 )
                {
                    appLogger.pushLog( std::this_thread::get_id(), "Client closed connection" ); 
                    closesocket(client);
                    client = INVALID_SOCKET;
                    break;
                }
                else if ( bytesRec < 0 )
                {
                     int code = WSAGetLastError();
                     std::string codeStr = std::to_string( code );
                     std::string log = "recv() failed, WSA error: " ;
                     log += codeStr;
                     appLogger.pushLog( std::this_thread::get_id(), log );
                     closesocket(client);   
                     client = INVALID_SOCKET;
                     break;
                }

                if ( accumlate.size() > 125000 )
                {
                    HTTP_ERROR(400, client);
                    appLogger.pushLog( std::this_thread::get_id(), msg.host, msg.method, 400,"Exceeded max header size", 0 );
                    closesocket(client);
                    client = INVALID_SOCKET;
                    break;
                }
                recvBuf[ bytesRec ] = '\0';
                std::string temp ( recvBuf, bytesRec );
                accumlate += temp;
                searchStart = (bytes >= 3)? bytes - 3 : 0;
                bytes += bytesRec;
                
                // incase /r/n/r/n was send sepreatly

                auto end = std::chrono::high_resolution_clock::now();
                auto duration = duration_cast<std::chrono::microseconds>(end - start);
                if ( duration > std::chrono::seconds(30) )
                {
                    HTTP_ERROR(400,client);
                    appLogger.pushLog( std::this_thread::get_id(), msg.host, msg.method, 400,"Exceeded max header sending time",0 );
                    closesocket(client);
                    client = INVALID_SOCKET;
                    break;
                }

                
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
            auto timeOutEnd = std::chrono::high_resolution_clock::now();
            auto timeOutDur = duration_cast<std::chrono::microseconds>(timeOutEnd - timeOut);
            if ( timeOutDur >= std::chrono::minutes(10) )
                msg.connection = "close";

            if ( msg.connection == "close" )
            {
                closesocket( client );
                break;
            }
        }


};


void httpServer::acceptAndServe ()
{
    while ( true )
    {
        SOCKET client = accept ( server, NULL, NULL );
        if ( client == INVALID_SOCKET )
            continue;
        DWORD timeout = 8000;
        setsockopt( client, SOL_SOCKET, SO_RCVTIMEO, (const char*) &timeout, sizeof(timeout) );
        BOOL nodelay = TRUE;
        // removes Nagle's algorithim , waits around 40ms to get an ACK, depreceted
        setsockopt( client, IPPROTO_TCP, TCP_NODELAY, (const char*) &nodelay, sizeof(nodelay) );
        thPool.pushTask( [  client, this ] { this -> serveRequest( client ); });

    }
    closesocket( server );


}

bool httpServer::validateRequest( httpRequest& msg, SOCKET& client )
{
    
            auto methodCheck = methodMap.find ( msg.method );

            if ( msg.version != "HTTP/1.1" && msg.version != "HTTP/1.0" )
            {
                if ( msg.version == "HTTP/2" || msg.version == "HTTP/3" )
                {
                    HTTP_ERROR(505, client);
                    appLogger.pushLog( std::this_thread::get_id(), msg.host, msg.method, 505,"Unsupported Version",0 );
                }
                else
                {
                    HTTP_ERROR( 400, client );
                    appLogger.pushLog( std::this_thread::get_id(), msg.host, msg.method, 400,"Invalid Version",0 );
                    msg.connection = "close";
                    return false;
                }

            }
            else if ( msg.host == "none" || msg.host == "" )
            {
                HTTP_ERROR ( 400, client );
                    appLogger.pushLog( std::this_thread::get_id(), msg.host, msg.method, 400,"Invalid Host",0 );
                msg.connection = "close";
                return false;
            }
            else if ( methodCheck == methodMap.end () )
            {
                HTTP_ERROR( 405, client );
                    appLogger.pushLog( std::this_thread::get_id(), msg.host, msg.method, 405,"Invalid Method",0 );
                msg.connection = "close";
                return false;
            }
            else if ( msg.encoding != "whole" )
            {
                if ( msg.encoding == "chunked" )
                {
                    HTTP_ERROR( 501, client );
                    appLogger.pushLog( std::this_thread::get_id(), msg.host, msg.method, 501,"Unsupported Encoding",0 );
                }
                else
                {
                    HTTP_ERROR( 400, client );
                    appLogger.pushLog( std::this_thread::get_id(), msg.host, msg.method, 400,"Invalid Encoding",0 );
                    msg.connection = "close";
                    return false;
                }

            }
            else if ( msg.contLength == "err" )
            {
                HTTP_ERROR( 400, client );
                appLogger.pushLog( std::this_thread::get_id(), msg.host, msg.method, 400,"Invalid Content Length",0 );
                msg.connection = "close";
                return false;
            }
            else if ( msg.host == "err" )
            {
                HTTP_ERROR( 400, client );
                appLogger.pushLog( std::this_thread::get_id(), msg.host, msg.method, 400,"Invalid Host",0 );
                msg.connection = "close";
                return false;
            }

            if ( msg.expect == "100-continue" )
            {
                std::string response = "HTTP/1.1 100 Continue\r\n"
                                       "\r\n";
                send ( client, response.c_str(), response.size(), 0 );  

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


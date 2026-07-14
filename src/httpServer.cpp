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
void httpServer::getRequest ( char* recBuf, httpRequest &msg, int &bytesRec )
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


        httpRequest msg {};
        while ( msg.connection == "open")
        {
            char* recvBuf = new char[4096];
            int bytesRec = recv( client, recvBuf, 4096, 0 );
            if ( bytesRec <= 0 ) 
            {
                std::cout<<"Error overflow in the recived request"<<std::endl;
                delete [] recvBuf;
                closesocket(client);

                break;
            }
            getRequest( recvBuf, msg, bytesRec );
            delete [] recvBuf;

            auto methodCheck = methodMap.find ( msg.method );
            if ( msg.version == "--" )
            {
                HTTP_ERROR(505, client);
                msg.connection = "close";

            }
            else if ( methodCheck == methodMap.end () )
            {
                HTTP_ERROR( 405, client );
            }
            else 
            {
                methodMap [ msg.method ] ( msg, client, contentPath );
            }
            if ( msg.connection == "close" )
                closesocket( client );
        }


                });
        worker.detach();
    }
    closesocket( server );


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


#include "../include/methods.hpp"
#include "../include/httpRequest.hpp"
#include "../include/maps.hpp"
#include <algorithm>
#include <cstddef>
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
#include <fileapi.h>
#include <winnt.h>


void HTTP_ERROR ( int code, SOCKET &client)
{
    std::string codeStr = std::to_string ( code );
    std::string status  = erros [ code ];
    std::string response = "HTTP/1.1 "+codeStr+status+"\r\n"
                           "Content-Type: application/json\r\n"
                           "Connection: close\r\n"
                           "\r\n";
    send ( client, response.c_str(), response.size(), 0 );  
}

void HTTP_GET ( httpRequest &msg, SOCKET &client, std::string& path )
{
    if ( msg.url == "--" )
    {
        HTTP_ERROR( 400, client );
        msg.connection = "close";
        return;
    }
    HANDLE hFile = CreateFile(
            (path + msg.url ).c_str(),
            GENERIC_READ,
            FILE_SHARE_READ,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL
            );
    if ( hFile == INVALID_HANDLE_VALUE )
    {
        std::cout<<"Error accessing file or it doesnt exist, code: "<<GetLastError()<<std::endl;
        HTTP_ERROR ( 404, client );
        // new
        msg.connection = "close";
    }
    else 
    {
        LARGE_INTEGER fileSize {};
        GetFileSizeEx( hFile , &fileSize );
        size_t index = msg.url.find( '.' ) + 1;
        std::string alias = msg.url.substr( index, msg.url.size() - index );
        std::cout<<alias<<" "<<conType[alias]<<std::endl;


        std::string header = "HTTP/1.1 200 OK\r\n"
                             "Content-Type: "+ conType[alias] +"\r\n"
                             "Content-Length: " + std::to_string( fileSize.QuadPart )+"\r\n" 
                             "Connection: keep-alive\r\n"
                             "\r\n";
        send ( client, header.c_str(), header.size(), 0 );
        bool success = TransmitFile ( client, hFile, 0, 0, NULL, NULL, 0 );
        if ( !success )
        {
            std::cout<<"Error sending the file"<<std::endl;
        }
        else 
            std::cout<<"File sent successfully"<<std::endl;

    }
    CloseHandle( hFile );
}

void HTTP_DELETE ( httpRequest &msg, SOCKET &client, std::string& path )
{
    std::string fullPath = path  + msg.url;
    LPCSTR file = fullPath.c_str();

    if ( DeleteFile( file ) )
    {
        std::cout<<"File deleted successfully "<<std::endl;

        size_t index = msg.url.find('.') + 1;
        std::string alias = msg.url.substr ( index, msg.url.size() - index );
        std::string header = "HTTP/1.1 204 No Content\r\n"
                             "Connection: keep-alive\r\n"
                             "Content-Length: 0\r\n" 
                             "\r\n" ;
        send ( client, header.c_str(), header.size(), 0 );   
    
    }
    else 
    {
        std::cout<<"failed to delete file, code: "<<GetLastError()<<std::endl;
        HTTP_ERROR ( 404, client );
        msg.connection = "close";
    }
    
        

}


void HTTP_PUT ( httpRequest &msg, SOCKET &client, std::string& path )
{
    std::string fullPath = path + msg.url;
    LPCSTR file = fullPath.c_str();

    HANDLE hFile = CreateFile(
            file,
            GENERIC_WRITE,
            0,
            NULL,
            CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            NULL
            );
    if ( hFile == INVALID_HANDLE_VALUE )
    {
        std::cout<<"Error accessing file, code: "<<GetLastError()<<std::endl;
        HTTP_ERROR( 500, client );

        msg.connection = "close";
        return ;
    }
    else 
    {
        std::cout<<"File accessed successfully"<<std::endl;

    }
    DWORD bytesToWrite = (DWORD) msg.body.size();
    DWORD bytesWritten = 0;
    while ( bytesWritten < bytesToWrite )
    {
        DWORD currentWrittenBytes = 0;
        bool res = WriteFile( hFile, msg.body.c_str() + bytesWritten,bytesToWrite - bytesWritten ,&currentWrittenBytes, 0 );
        if ( !res)
        {
            std::cout<<"Error writting to file, code: "<<GetLastError()<<std::endl;
            HTTP_ERROR( 500, client );
            msg.connection = "close";
            CloseHandle ( hFile );
            return;
        }
        bytesWritten += currentWrittenBytes;
    }
    std::cout<<msg.contLength<<std::endl;
    int remainBytes = std::stoi( msg.contLength ) - msg.body.size();
    int bufferSize = std::min<int>( 32000, remainBytes );
    char* buffer = new char [ bufferSize ];
    bytesWritten = 0;
    while ( bytesWritten < remainBytes )
    {
        int recBytes = recv( client, buffer, std::min<int>(bufferSize, std::max<int>(remainBytes - bytesWritten,0) ), 0 );
        if ( recBytes <= 0)
        {
            std::cout<<"Premature end of connection while writing to file, code: "<<WSAGetLastError()<<std::endl;
            HTTP_ERROR(  500, client );
            msg.connection = "close";
            CloseHandle ( hFile );
            delete [] buffer;
            return;
        }
        DWORD currentWrittenBytes = 0;
        while ( currentWrittenBytes < recBytes )
        {
            DWORD oneCallBytes = 0;
            bool success = WriteFile( hFile, buffer + currentWrittenBytes, recBytes - currentWrittenBytes, &oneCallBytes, 0 );
            if ( !success )
            {
                std::cout<<"Error Writing to file, code: "<<GetLastError()<<std::endl;
                HTTP_ERROR ( 500, client );
                CloseHandle( hFile );
                delete [] buffer;
                return;

            }
            bytesWritten += oneCallBytes;
            currentWrittenBytes += oneCallBytes;
        }
    }
        std::cout<<"successfully wrote to file"<<std::endl;
        std::string header = "HTTP/1.1 201 Created\r\n"    
                             "Content-Length: 0\r\n" 
                             "Connection: keep-alive\r\n"                                           
                             "\r\n";
        send(client, header.c_str(), header.size(), 0); 
                          
        delete [] buffer;
        CloseHandle ( hFile );


}
void headerConnection ( std::string &value, httpRequest &msg )
{ 
    ( value == "close" )?
        msg.connection = "close"
    :
        msg.connection = "open";
}
void headerLength ( std::string &value, httpRequest &msg )
{

    msg.contLength = value;


}


void parseHeaders ( std::string &headers, httpRequest &msg )
{
    size_t index = 0;
    while ( index < headers.size () )
    {
        size_t endLine = headers.find( "\r\n", index );
        if ( endLine == std::string::npos || endLine == index ) break;
        size_t colon = headers.find( ':', index );
        if ( colon == std::string::npos || colon > endLine )
        {
            index = endLine + 2;
            continue;
        }
        size_t typeStart = index;
        size_t typeEnd   = colon;
        while ( typeStart < typeEnd && headers[ typeStart ] == ' ') ++typeStart;
        while ( typeEnd   > typeStart && headers[ typeEnd - 1 ] == ' ') --typeEnd;
        std::string type = headers.substr ( typeStart, typeEnd - typeStart );
        size_t valueStart = colon + 1;
        size_t valueEnd   = endLine;
        while ( valueStart < valueEnd && headers[ valueStart ] == ' ' ) ++valueStart;
        while ( valueEnd   > valueStart && headers[ valueEnd - 1] == ' ' ) --valueEnd;
        std::string value = headers.substr ( valueStart, valueEnd - valueStart );
        if ( headerMap.find( type ) != headerMap.end() )
        {
            
            std::cout<<type<<" "<<value<<std::endl;
            headerMap[ type ] ( value, msg );
        }

        std::cout<<"("<<type<<")"<<"("<<value<<")"<<std::endl;

        index = endLine + 2;
    }
            

        
}
    

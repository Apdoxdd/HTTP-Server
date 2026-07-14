#include "../include/httpRequest.hpp"
#include <iostream>
#include <string>


void httpRequest::extractMethod ( std::string& msg )
{
    size_t end = msg.find( ' ' );
    if( end == std::string::npos)
    {
        method = "";
        return;
    }
    method = msg.substr( 0, end ); 
}


void httpRequest::extractURL ( std::string& msg )
{
    size_t start = msg.find( ' ' );
    if( start == std::string::npos )
    {
        url = "--";
        return;
    }
    start += 2;
    size_t end   = msg.find( ' ', start );
    if( end == std::string::npos )
    {
        url = "--";
        return;
    }
    url = msg.substr( start, end - start );
    size_t sant = url.find( "../" );
    size_t sant2 = url.find( "..\\");
    if ( sant != std::string::npos || sant2 != std::string::npos )
    {
        url = "--";
        return;
    }
    if ( url.empty() )
        url = "index.html";
}

void httpRequest::extractVersion ( std::string& msg )
{
    size_t start = msg.find( ' ' );
    if ( start == std::string::npos )
    {
        version = "--";
        return;
    }
    start++;
    start = msg.find ( ' ', start );
    
    if ( start == std::string::npos )
    {
        version = "--";
        return;
    }
    start++;
    size_t end = msg.find( "\r\n" );

    if ( end == std::string::npos )
    {
        version = "--";
        return;
    }
    version = msg.substr( start, end - start );
}

void httpRequest::extractHeaders ( std::string& msg )
{
    size_t start = msg.find( "\r\n" );
    
    if ( start == std::string::npos )
    {
        return;
    }
    start += 2;
    size_t end   = msg.find( "\r\n\r\n" );

    if ( end == std::string::npos )
    {
        return;
    }
    headers = msg.substr( start, end - start + 2 );
}

void httpRequest::extractBody ( std::string& msg )
{
    size_t start = msg.find( "\r\n\r\n" );
   
    if ( start == std::string::npos )
    {
        return;
    }
    start += 4;
    body = msg.substr ( start );
}

httpRequest::httpRequest ():
   connection( "open" ),
   contLength( "0" )
{}


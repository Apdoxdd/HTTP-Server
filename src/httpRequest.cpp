#include "../include/httpRequest.hpp"
#include <iostream>
#include <string>


void httpRequest::extractMethod ( std::string& msg )
{
    size_t end = msg.find( ' ' );
    method = msg.substr( 0, end ); 
}


void httpRequest::extractURL ( std::string& msg )
{
    size_t start = msg.find( ' ' ) + 2;
    size_t end   = msg.find( ' ', start );
    url = msg.substr( start, end - start );
    if ( url.empty() )
        url = "index.html";
}

void httpRequest::extractVersion ( std::string& msg )
{
    size_t start = msg.find( ' ' ) + 1;
    start = msg.find ( ' ', start ) + 1;
    size_t end = msg.find( "\r\n" );
    version = msg.substr( start, end - start );
}

void httpRequest::extractHeaders ( std::string& msg )
{
    size_t start = msg.find( "\r\n" ) + 2;
    size_t end   = msg.find( "\r\n\r\n" );
    headers = msg.substr( start, end - start + 2 );
}

void httpRequest::extractBody ( std::string& msg )
{
    size_t start = msg.find( "\r\n\r\n" ) + 4;
    body = msg.substr ( start );
}

httpRequest::httpRequest ():
   connection( "open" ),
   contLength( "0" )
{}


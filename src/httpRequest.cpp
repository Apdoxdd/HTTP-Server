#include "../include/httpRequest.hpp"
#include <string>
#include <vector>
#include <algorithm>
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

    std::transform( url.begin(), url.end(), url.begin(), [](unsigned char c)
            {return std::tolower (c);});
    // not an exhaustive list but works well for most attempts
    std::vector<std::string> byPass = {
       "../", 
       "..\\",
       "%2e%2e%2f",
       "%2e%2e/",
       "..%2f",
       "%2e%2e%5c",
       "%2e%2e\\",
       "%252e%252e%252f",
       "%252e%252e/",
       "%c0%ae%c0%ae%c0%af",
       "%uff0e%uff0e%uff0f",
    };
    bool found = std::any_of( byPass.begin(), byPass.end(), [&] ( const std::string& pass) {
            return url.find( pass ) != std::string::npos;
            });
    if ( found )
    {
        url = "--";
        return;
    }
    if ( url.empty() )
        url = "index.html";
    else 
    {

        if ( url.find( ':' ) != std::string::npos ||
               url.substr(0, 2) == "\\\\" || url.back() == '.' || url.back() == ' ' )
        {
            url = "--";
                return;
        }
    }
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
   contLength( "0" ),
   host ( "none" )
{}


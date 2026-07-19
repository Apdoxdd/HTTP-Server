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
    size_t pos = 0;
    std::vector<std::string> allowedEnc = {
            "%20", "%3f", "%23", "%26", "%40"
    };
    while ( ( pos = url.find( '%', pos ) ) != std::string::npos )
    {
        std::string cand = url.substr (pos ,3 ); //  %26 = & which is allowed
        bool isAllowed = std::any_of( allowedEnc.begin(), allowedEnc.end(), [&] (const std::string& pass ){
        return cand == pass;});
        if ( !isAllowed )
            {
                url = "--";
                return;
            }
            pos += 3;
    }
                

    // not an exhaustive list but works well for most attempts
    std::vector<std::string> byPass = {
       "../", 
       "..\\",
       "//"
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


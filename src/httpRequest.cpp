#include "../include/httpRequest.hpp"
#include <string>
#include <vector>
#include <algorithm>
size_t httpRequest::extractMethod ( std::string& msg, size_t lead)
{
    size_t end = msg.find( ' ', lead );
    if( end == std::string::npos)
    {
        method = "";
        return end;
    }
    method = msg.substr( lead, end - lead );
    return end + 1;
}


size_t httpRequest::extractURL ( std::string& msg, size_t lead )
{
    // GET /index.html
    //     |
    //     lead
    size_t end   = msg.find( ' ', lead );
    lead++;
    if( end == std::string::npos )
    {
        url = "--";
        return end;
    }
    url = msg.substr( lead, end - lead );
    //windows donest care about casing so thats ok
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
                return std::string::npos;
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
        return std::string::npos;
    }
    if ( url.empty() )
        url = "index.html";
    else 
    {

        if ( url.find( ':' ) != std::string::npos ||
               url.substr(0, 2) == "\\\\" || url.back() == '.' || url.back() == ' ' )
        {
            url = "--";
                return std::string::npos;
        }
    }
    return end + 1;
}

size_t httpRequest::extractVersion ( std::string& msg, size_t lead )
{
    size_t end = msg.find( "\r\n", lead );

    if ( end == std::string::npos )
    {
        version = "--";
        return end;
    }
    version = msg.substr( lead, end - lead );
    return end + 2;
}

size_t httpRequest::extractHeaders ( std::string& msg, size_t lead)
{
    size_t end   = msg.find( "\r\n\r\n", lead );

    if ( end == std::string::npos )
    {
        return end;
    }
    headers = msg.substr( lead, end - lead + 2 );
    return end + 4; 
}

size_t httpRequest::extractBody ( std::string& msg, size_t lead)
{
    if ( lead == std::string::npos || lead > msg.size() )
    {
        body = "";
        return std::string::npos;
    }
    body = msg.substr ( lead );
    return 1;
}

httpRequest::httpRequest ():
   connection( "open" ),
   contLength( "0" ),
   host ( "none" ),
   encoding( "whole" ),
   expect("none")
{}


// doctest version: 2.5.0
// source: https://github.com/doctest/doctest
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "../../include/httpRequest.hpp"
#include "../../dependency/doctest/doctest.h"


TEST_CASE( "extractURL rejects classic traversal") 
{
    httpRequest req;
    std::string raw = "GET ../../windows/system32 HTTP/1.1\r\n\r\n";
    size_t i = req.extractMethod( raw, 0);
    i = req.extractURL( raw, i );
    CHECK( req.getURL() == "--" );
}

TEST_CASE( "extract URL accepts a normal path" )
{
    httpRequest req;
    std::string raw = "GET /index.html HTTP/1.1\r\n\r\n";
    size_t i = req.extractMethod( raw, 0 );
    i = req.extractURL( raw, i );
    CHECK( req.getURL() == "index.html" );
}

TEST_CASE( "extract URL rejects an unrecognized percent-encoding" )
{
    httpRequest req;
    std::string raw = "GET /%2e%2e HTTP/1.1\r\n\r\n";
    size_t i = req.extractMethod( raw, 0);
    i = req.extractURL( raw, i );
    CHECK( req.getURL() == "--" );
}
TEST_CASE( "extractHeader accepts the entire header block without any modification" )
{
    httpRequest req;
    std::string raw = "POST /api/users HTTP/1.1\r\n"
                      "Host: example.com\r\n"
                      "Content-Type: application/json\r\n"
                      "Content-Length: 34\r\n"
                      "Authorization: Bearer MY_TOKEN\r\n"
                      "\r\n";
    std::string header = 
                      "Host: example.com\r\n"
                      "Content-Type: application/json\r\n"
                      "Content-Length: 34\r\n"
                      "Authorization: Bearer MY_TOKEN\r\n";

    size_t i = req.extractMethod( raw, 0 );
    i = req.extractURL( raw, i );
    i = req.extractVersion( raw , i );
    i = req.extractHeaders( raw, i );
    CHECK( req.getHeaders() == header );
}

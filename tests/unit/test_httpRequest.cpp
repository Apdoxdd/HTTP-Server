// doctest version: 2.5.0
// source: https://github.com/doctest/doctest
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "../../include/httpRequest.hpp"
#include "../../dependency/doctest/doctest.h"


TEST_CASE( "extractMethod extracts a valid HTTP method" )
{
    httpRequest req;
                    
    std::string raw = "POST /cat.jpg HTTP/1.1\r\n\r\n";

    size_t i = req.extractMethod( raw, 0 );

    CHECK( req.getMethod() == "POST" );
    CHECK( i == 5 );
}

TEST_CASE( "extractURL rejects classic traversal") 
{
    httpRequest req;
    std::string raw = "GET ../../windows/system32 HTTP/1.1\r\n\r\n";
    size_t i = req.extractMethod( raw, 0);
    i = req.extractURL( raw, i );
    CHECK( req.getURL() == "--" );
}

TEST_CASE( "extractURL converts root URL to index.html" )
{
    httpRequest req;
    std::string raw = "GET / HTTP/1.1\r\n\r\n";

    size_t i = req.extractMethod( raw, 0 );
    i = req.extractURL( raw, i );

    CHECK( req.getURL() == "index.html" );
}

TEST_CASE( "extractURL converts URL to lowercase" )
{
    // important since windows files dont care about upper and lower casing
    httpRequest req;

    std::string raw = "GET /INDEX.HTML HTTP/1.1\r\n\r\n";

    size_t i = req.extractMethod( raw, 0 );
    i = req.extractURL( raw, i );

    CHECK( req.getURL() == "index.html" );
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

TEST_CASE( "extractVersion extracts HTTP version" )
{
    httpRequest req;

    std::string raw = "GET /index.html HTTP/1.1\r\n\r\n";

    size_t i = req.extractMethod( raw, 0 );
    i = req.extractURL( raw, i );
    i = req.extractVersion( raw, i );

    CHECK( req.getVersion() == "HTTP/1.1" );
}

TEST_CASE( "extractHeaders rejects incomplete header block" )
{
    httpRequest req;

    std::string raw = "GET /index.html HTTP/1.1\r\n"
                                "Host: localhost\r\n";

    size_t i = req.extractMethod( raw, 0 );
    i = req.extractURL( raw, i );
    i = req.extractVersion( raw, i );
    i = req.extractHeaders( raw, i );
    CHECK( i == std::string::npos );
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


TEST_CASE( "extractBody extracts the remaining request body" )
{
        httpRequest req;

            std::string raw =
                        "GET /index.html HTTP/1.1\r\n"
                                "Content-Length: 11\r\n"
                                        "\r\n"
                                                "Hello World";

                size_t i = req.extractMethod( raw, 0 );
                    i = req.extractURL( raw, i );
                        i = req.extractVersion( raw, i );
                            i = req.extractHeaders( raw, i );
                                i = req.extractBody( raw, i );

                                    CHECK( req.getBody() == "Hello World" );
}

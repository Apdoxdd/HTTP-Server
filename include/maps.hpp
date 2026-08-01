#ifndef MAPS
#define MAPS

#include "../include/httpRequest.hpp"
#include "../include/httpServer.hpp"
#include <string>
#include <shared_mutex>
#include <mutex>
#include <unordered_map>
#include "methods.hpp"

//even tho many threads access these maps its okay in this specific case as we only ever read
// and stl by default supports multiple concerent reads

inline std::unordered_map <std::string,std::string> conType = {
    //text  and markup
        {"html","text/html"},
        {"htm","text/html"},
        {"txt","text/plain"},
        {"css","text/css"},
        {"csv","text/csv"},
        {"xml","application/xml"},

        // scripts and data
        {"js","application/javascript"},
        {"json","application/json"},

        // imgages 
        {"jpg","image/jpeg"},
        {"jpeg","image/jpeg"},
        {"png","image/png"},
        {"gif","image/gif"},
        {"svg","image/svg+xml"},
        {"webp","image/webp"},

        //documents
        {"pdf","application/pdf"},

        //audio and video
        {"mp3","audio/mpeg"},
        {"mp4","video/mp4"},
        {"wav","audio/wav"}

    };
inline std::mutex locksMtx;

inline std::unordered_map<std::string, std::shared_mutex> fileLocks { 

};

inline std::unordered_map< int, std::string >erros = {
    { 405, " Method Not Allowed" },
    { 400, " Bad Request" },
    { 500, " Internal Server Error" },
    { 505, " Version Not Supported"  },
    { 404, " Not Found" },
    { 501, " Not Implemented"},
    { 413, " Content Too Large"}
};

inline std::unordered_map<std::string, void (*) (httpRequest&, SOCKET&, std::string&, httpServer&)> methodMap {
    {"GET",     HTTP_GET},
    {"DELETE",  HTTP_DELETE},
    {"PUT",     HTTP_PUT},
    {"HEAD",    HTTP_HEAD},
    {"POST",    HTTP_POST}
};

inline std::unordered_map<std::string, void(*) (std::string&, httpRequest&)> headerMap {
    {"connection", headerConnection},
    {"content-length", headerLength},
    {"host", headerHost  },
    {"transfer-encoding", headerEncoding},
    {"expect", headerExpect}
};






#endif

#ifndef MAPS
#define MAPS

#include "../include/httpRequest.hpp"
#include <string>
#include <unordered_map>
#include "methods.hpp"


inline std::unordered_map <std::string,std::string> conType = {
        {"html","text/html"},
        {"txt","text/plain"},
        {"jpg","image/jpeg"},
    };


inline std::unordered_map< int, std::string >erros = {
    { 405, " Method Not Allowed" },
    { 400, " Bad Request" },
    { 500, " Internal Server Error" },
    { 505, " Version Not Supported"  },
    { 404, " Not Found" }
};

inline std::unordered_map<std::string, void (*) (httpRequest&, SOCKET&, std::string&)> methodMap {
    {"GET",     HTTP_GET},
    {"DELETE",  HTTP_DELETE},
    {"PUT",     HTTP_PUT}
};

inline std::unordered_map<std::string, void(*) (std::string&, httpRequest&)> headerMap {
    {"connection", headerConnection},
    {"content-length", headerLength},
    {"host", headerHost  }

};






#endif

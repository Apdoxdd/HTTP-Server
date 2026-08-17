#ifndef FILE_CACHE
#define    FILE_CACHE

#include <string>
#include <unordered_map>
#include <shared_mutex>

struct cachedFile

{
    std::string content;
    std::string contentType;
};

inline std::shared_mutex cacheMtx;
inline std::unordered_map< std::string, cachedFile > fileCache;














#endif

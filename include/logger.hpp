#ifndef LOGGER
#define LOGGER

#include <string>
#include <mutex>
#include <atomic>
#include <queue>
#include <thread>
#include <condition_variable>

class logger 
{
private:
    std::condition_variable logCv;
    std::queue<std::string>logsQ;
    std::mutex mtx;
public:
    std::atomic<bool>stop;

    logger();
    void pushLog( std::thread::id thID, std::string host, std::string method = "", int code = -1, std::string serverMsg = "", bool success = 1);
    void logRelayLoop();
    void deliverLogBatch( std::string logBatch );
    ~logger();
private:
    std::thread logThread;
    // memebers get intiallized based on where they appear in declaration, since logThrad checks stop it needs to be below it
};

inline logger appLogger;
// global varaible that's inline so that it doesnt appear differnet between 2 different translation units, and logs without changing the signature of all functions

#endif

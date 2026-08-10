#include <string>
#include <mutex>
#include <atomic>
#include <queue>
#include <thread>
#include <format>
#include <iostream>

#include "../include/logger.hpp"

logger::logger():  logThread( &logger::logRelayLoop, this ), stop ( false )
{}


void logger::pushLog( std::thread::id thID, std::string host, std::string method, int code,std::string serverMsg, bool success)
{   
    if ( code != -1 )
    {
        std::string status = ( success )? "Success with code: ":  "Error with code: ";
        status += std::to_string(code);
        std::string threadID = std::format("{}", thID);
        std::string log { status + ", " + " Thread: " +threadID +" served the host: "+ host + ", method: "+ method +" ,server log: "+ serverMsg + "\n" }; 
        { 
            std::lock_guard<std::mutex>lk( mtx );
            logsQ.push( log );
        }
    }
    else 
    {
        std::string threadID = std::format( "{}", thID );
        std::string log { "Thread " + threadID +": " + host + "\n"};
        { 
            std::lock_guard<std::mutex>lk( mtx );
            logsQ.push( log );
        }
    }
    logCv.notify_one();

}

void logger::logRelayLoop()
{
    while( true )
    {
        std::unique_lock<std::mutex>lk( mtx );
        logCv.wait( lk, [&](){ return !logsQ.empty() || stop; } );
        if ( logsQ.empty() )
            return;
        auto log = logsQ.front();
        logsQ.pop();
        lk.unlock();
        deliverLogBatch( log );
    }
}

void logger::deliverLogBatch( std::string logBatch )
{
    std::cout<<logBatch<<std::endl; 
}

logger::~logger()
{
    stop = true;
    logCv.notify_one();
    logThread.join();
}


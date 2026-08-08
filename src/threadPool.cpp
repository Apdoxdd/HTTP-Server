#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <vector>
#include <queue>
#include <functional>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include "../include/threadPool.hpp"


threadPool::threadPool(): stop ( false )
{
    unsigned int minNum = std::thread::hardware_concurrency();
    poolSize = ( minNum == 0 )? 2 : minNum;

    for ( auto i {0uz}; i < poolSize; ++i )
    {
        pool.emplace_back( &threadPool::doTask, this );
    }
    
}

void threadPool::doTask()
{
    while( true )
    {
        std::unique_lock<std::mutex>lk( mtx );
        cv.wait( lk, [&](){ return !taskQ.empty() || stop; }); 
        if ( taskQ.empty() )
            return;
        // you already hold lk the only way taskQ could be empty
        // but still cv notifies us is if stop is true
        auto task = taskQ.front();
        taskQ.pop();
        lk.unlock();
        task();

    }
}
void threadPool::pushTask ( std::function<void()>task )
{
    {
        std::lock_guard<std::mutex>lk ( mtx );
        taskQ.push( task );
    }
    cv.notify_all();

}
threadPool::~threadPool()
{
    stop = true;
    cv.notify_all();
    for ( auto &thread : pool )
        thread.join();
}

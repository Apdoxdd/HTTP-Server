#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <vector>
#include <queue>
#include <functional>
#include "../include/threadPool.hpp"


threadPool::threadPool(): stop ( false )
{
    // after testing i found that threads spend most of their time blocked on recv and transfile
    // since thats the case, its safe to add a few hundred threads to the pool without the worry of
    // interleaving since the threads spend most of their time waiting instead of doing actual work
    // to fix that for all we need to use IOCP ( in the future mby )
    unsigned int minNum = std::thread::hardware_concurrency() * 50;
    //  after binary searching through threads number of * 100 to *2 , *50 was by far the best performing
    //  and the one with least interleaving
    poolSize = ( minNum == 0 )? 100 : minNum;

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

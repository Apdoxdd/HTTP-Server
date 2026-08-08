#ifndef THREAD_POOL
#define THREAD_POOL

#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <vector>
#include <queue>
#include <functional>

class threadPool 
{
private:
    std::mutex mtx;
    std::condition_variable cv;
    std::vector<std::thread>pool;
    std::queue<std::function<void()>>taskQ;
    unsigned int poolSize;

public:
    std::atomic<bool>stop;
    threadPool();
    void doTask();
    void pushTask( std::function<void()>task );
    ~threadPool();

};

#endif

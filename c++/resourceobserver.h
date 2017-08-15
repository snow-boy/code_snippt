#ifndef RESOURCEOBSERVER_H
#define RESOURCEOBSERVER_H

#include <atomic>
#include <mutex>
#include <condition_variable>


class ResourceObserver
{
public:
    ResourceObserver():
        count_(0)
    {}
	 
    void Produce()
    {
        std::lock_guard<std::mutex> lock(res_mutex_);
        count_ ++;
        cond_var_.notify_one();
    }

    void Consume()
    {
        std::unique_lock<std::mutex> lock(res_mutex_);
        cond_var_.wait(lock, [this]{
            return count_.load() > 0;
        });
        count_ --;
    }

private:
    std::atomic_int32_t count_;
    std::mutex res_mutex_;
    std::condition_variable cond_var_;
};


#endif // RESOURCEOBSERVER_H

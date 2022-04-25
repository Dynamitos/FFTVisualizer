#pragma once
#include "Resources.h"
#include <queue>
#include <mutex>
#include <atomic>
#include <memory>
template<typename T>
class PacketQueue
{
public:
    PacketQueue()
    {
        bIsFinished.store(false);
    }
    virtual ~PacketQueue()
    {

    }
    void signalFinished()
    {
        std::scoped_lock lock(queueLock);
        bIsFinished.store(true);
        queueCV.notify_all();
    }
    void addToQueue(std::unique_ptr<T> packet)
    {
        std::scoped_lock lock(queueLock);
        queue.push(std::move(packet));
        queueCV.notify_one();
    }
    std::unique_ptr<T> popPacket()
    {
        std::unique_lock lock(queueLock);
        while (queue.empty()) 
        { 
            if(bIsFinished.load())
                return nullptr;
            queueCV.wait(lock);
        }
        std::unique_ptr<T> element = std::move(queue.front());
        queue.pop();
        return element;
    }
    constexpr int getQueueSize()
    {
        return queue.size();
    }
private:
    std::atomic_bool bIsFinished;
    std::mutex queueLock;
    std::condition_variable queueCV;
    std::queue<std::unique_ptr<T>> queue;
};
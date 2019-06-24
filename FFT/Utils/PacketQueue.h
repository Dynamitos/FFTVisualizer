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
		bIsFinished.store(true);
	}
	inline bool isFinished()
	{
		return bIsFinished.load() && queue.size() == 0;
	}
	void addToQueue(std::unique_ptr<T> packet)
	{
		std::lock_guard lock(queueLock);
		queue.push(std::move(packet));
	}
	std::unique_ptr<T> popPacket()
	{
		while (queue.size() == 0) std::this_thread::yield();
		std::lock_guard lock(queueLock);
		std::unique_ptr<T> element = std::move(queue.front());
		queue.pop();
		return element;
	}
private:
	std::atomic_bool bIsFinished;
	std::mutex queueLock;
	std::queue<std::unique_ptr<T>> queue;
};
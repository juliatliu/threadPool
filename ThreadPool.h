#ifndef JULIATLIU_THREAD_POOL_H
#define JULIATLIU_THREAD_POOL_H

#include <memory>
#include <mutex>
#include <condition_variable>
#include <deque>
#include <thread>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "Condition.h"
#include "Lock.h"
#include "atomic.h"
#include "LockFreeQueue.h"

namespace julia
{

class ThreadPool 
{

public:
	ThreadPool(const ThreadPool&) = delete;
	ThreadPool& operator = (const ThreadPool&) = delete;
	using LockGuardT = julia::LockGuard<PthreadMutexLock>;
	typedef boost::function<void ()> Task;

	ThreadPool(const std::string& name = std::string());

	~ThreadPool();

	void start(int numThreads);

	void stop();

	void run(const Task& task);


private:
	void runInThread();
	static void* startThread(void* obj);
	Task take();

	PthreadMutexLock m_mutex;
	PthreadCondition m_condition;
	std::string m_name;
	CAQueue<Task> m_queue;

	boost::ptr_vector<std::thread> m_threads;
	std::atomic<bool> m_running;
};

}

#endif

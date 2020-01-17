#include "ThreadPool.h"
#include <sys/syscall.h>
#include <iostream>
#include <boost/bind.hpp>
namespace julia
{

ThreadPool::ThreadPool(const std::string& name)
	: m_mutex()
	, m_condition(m_mutex)
	, m_name( name )
	, m_queue(100)
	, m_running( false )
{}

ThreadPool::~ThreadPool()
{
	if( m_running )
	{
		stop();
	}
}

void ThreadPool::start(int numThreads)
{
	assert(m_threads.empty());
	m_running = true;
	m_threads.reserve(numThreads);
	for(int i = 0; i < numThreads; ++i) {
		m_threads.push_back( new std::thread(
			boost::bind(&ThreadPool::startThread, this)		
		)); 
	}
}

void* ThreadPool::startThread(void* obj) 
{ 
	ThreadPool* threadPool =  static_cast<ThreadPool*>(obj); 
	threadPool->runInThread(); 
    return NULL; 
}

void ThreadPool::stop()
{
	m_running = false;
	m_condition.notify_all();
	std::for_each( std::begin(m_threads), std::end(m_threads),
		[](std::thread& t){
			t.join();
	});
}

void ThreadPool::run(const Task& task)
{
	if(m_threads.empty()) {
		task();
	} else {
		LockGuardT lock(m_mutex);
		bool ret = m_queue.push(task);
		if(ret) {
			m_condition.notify_one();
		}
	}
}

void ThreadPool::runInThread()
{
	try{
		while(m_running) {
			Task task(take());
			if(task)
			{
				task();
			}
		}
	} catch( const std::exception& ex ) {
		std::cerr << "exception caught in ThreadPool:" << m_name
			<< " reason:" << ex.what()
			<< std::endl;
		abort();
	} catch( ... ) {
		std::cerr << "unknown exception caught in ThreadPool:" << m_name
			<< std::endl;
		abort();
	}
}

ThreadPool::Task ThreadPool::take()
{
	LockGuardT lock(m_mutex);
	while( m_queue.empty() && m_running )
	{
		m_condition.wait();
	}
	Task task;
	bool ret = m_queue.pop(task);
	if(ret){
		return task;
	}
	else
		return Task();
}

}

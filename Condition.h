#include <pthread.h>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include "Lock.h"

namespace julia
{

class ICondition
{
public:
	ICondition() {}
	ICondition(const ICondition&) = delete;
	ICondition& operator = (const ICondition&) = delete;
	virtual ~ICondition() {}
	virtual void notify_one() = 0;
	virtual void notify_all() = 0;
	virtual void wait() = 0;
};

class PthreadCondition : public ICondition
{
public:
	explicit PthreadCondition(PthreadMutexLock& mutex)
		: m_mutex(mutex)
	{
		pthread_cond_init(&m_condition, NULL);
	}
	~PthreadCondition()
	{
		pthread_cond_destroy(&m_condition);
	}

	void wait() override final
	{
		pthread_cond_wait(&m_condition, m_mutex.getPthreadMutex());
	}

	void notify_one() override final
	{
		pthread_cond_signal(&m_condition);
	}

	void notify_all() override final
	{
		pthread_cond_broadcast(&m_condition);
	}

private:
	PthreadMutexLock& m_mutex;
	pthread_cond_t m_condition;
};


class Condition: public ICondition
{
public:
	Condition(std::mutex& mutex)
		: m_mutex(mutex)
	{};
	template<class Predicate>
	void wait(Predicate pred)
	{
		std::unique_lock<std::mutex> lockGuard(m_mutex);
		m_condition.wait(lockGuard, pred);

	}
	void wait() override final
	{
		std::unique_lock<std::mutex> lockGuard(m_mutex);
		m_condition.wait(lockGuard);

	}

	void notify_one() override final
	{
		std::unique_lock<std::mutex> lockGuard(m_mutex);
		m_condition.notify_one();
	}

	void notify_all() override final
	{
		std::unique_lock<std::mutex> lockGuard(m_mutex);
		m_condition.notify_all();
	}
private:
	std::mutex& m_mutex;
	mutable std::condition_variable m_condition;
};

}

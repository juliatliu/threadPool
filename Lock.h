#ifndef JULIATLIU_LOCK_H
#define JULIATLIU_LOCK_H

#include <pthread.h>
#include <atomic>
#include <assert.h>

namespace julia
{

#define likely(x) __builtin_expect(!!(x), true)
#define unlikely(x) __builtin_expect(!!(x), false)

class Lock
{
public:
	Lock() {};
	Lock(const Lock&) = delete;
	Lock& operator = ( const Lock& ) = delete;
	virtual ~Lock() {}

	virtual bool lock() = 0;
	virtual bool unlock() = 0;
};

// traditional spin lock
class PthreadSpinLock : public Lock
{
public:
	PthreadSpinLock();
	virtual ~PthreadSpinLock();

	// try to obtain the lock
	bool lock() override final;

	// release the lock
	bool unlock() override final;

private:
	pthread_spinlock_t m_lock;
};


class PthreadMutexLock : public Lock
{
public:
	PthreadMutexLock()
		: m_pid(0)
	{
		pthread_mutex_init(&m_mutex, NULL);
	}
	~PthreadMutexLock();
	pthread_mutex_t* getPthreadMutex();

	bool isLockedByThisThread();
	bool lock() override final;
	bool unlock() override final;
private:
	pthread_mutex_t m_mutex;
	pid_t m_pid;
};


// lock free spin lock
class AtomicSpinLock : public Lock
{
public:
	AtomicSpinLock();
	bool lock()  override final
	{
		// acquire lock , and spin
		// test_and_set is atomic 
		while( m_lock.test_and_set(std::memory_order_acquire) ) {}
		return true;
	}
	bool unlock() override final
	{
		m_lock.clear( std::memory_order_release );
		return true;
	}
private:
	// atomic_flag is the only type that is guaranteed to be lock free
	// operations on std::atomic_bool are not lock free
	std::atomic_flag m_lock;
};


template<typename LockType>
class LockGuard
{
public:
	LockGuard( LockType& lock )
		: m_lock(lock)
	{
		assert(m_lock.lock());
	}
	~LockGuard()
	{
		assert(m_lock.unlock());
	}
private:
	LockType& m_lock;
};



}


#endif

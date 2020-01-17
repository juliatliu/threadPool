#include <sys/syscall.h>
#include <unistd.h>
#include "Lock.h"



namespace julia {

PthreadSpinLock::PthreadSpinLock(){
	assert( pthread_spin_init( &m_lock, PTHREAD_PROCESS_SHARED ) == 0 );
}

PthreadSpinLock::~PthreadSpinLock()
{
	pthread_spin_destroy( &m_lock );
}

bool PthreadSpinLock::lock() {
	if( unlikely( pthread_spin_lock( &m_lock ) != 0 ) ) {
		return false;
	}
	return true;
} 

bool PthreadSpinLock::unlock() {
	if( pthread_spin_unlock( &m_lock ) ) {
		return false;
	}
	else
	{
		return true;
	}
}


PthreadMutexLock::~PthreadMutexLock()
{
	assert(m_pid == 0);
	pthread_mutex_destroy(&m_mutex);
}

pthread_mutex_t* PthreadMutexLock::getPthreadMutex()
{
	return &m_mutex;
}

bool PthreadMutexLock::isLockedByThisThread()
{
	return m_pid == static_cast<pid_t>(::syscall(SYS_gettid));   
}

bool PthreadMutexLock::lock()
{
	pthread_mutex_lock(&m_mutex);
	m_pid = static_cast<pid_t>(::syscall(SYS_gettid));
	return true;
}

bool PthreadMutexLock::unlock()
{
	pthread_mutex_unlock(&m_mutex);
	m_pid = 0;
	return true;
}




AtomicSpinLock::AtomicSpinLock()
	: m_lock(ATOMIC_FLAG_INIT)
{}

}

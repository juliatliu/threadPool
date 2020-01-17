#ifndef JULIATLIU_LOCK_FREE_QUEUE_H
#define JULIATLIU_LOCK_FREE_QUEUE_H

#include <sched.h>
#include <atomic>
#include <stdio.h>

template<typename T>
class CAQueue {
public:
	CAQueue(uint32_t max_size)
		: m_head(0)
		, m_tail(0)
		, m_max_read_index(0)
		, m_capacity(max_size)
	{
	}

	bool push(const T& t);
	bool pop(T& t);
	bool empty() const {
		return m_head == m_tail;
	}
private:
	uint32_t m_head;
	uint32_t m_tail;
	uint32_t m_max_read_index;
	uint32_t m_capacity;
	T m_queue[100];
};

template <typename T>
bool CAQueue<T>::push(const T& t)
{
	uint32_t old_tail;
	uint32_t new_tail;
	do {
		old_tail = m_tail;
		new_tail = (old_tail + 1) % m_capacity;
		if(new_tail == m_head) {
			return false;
		}
	} while( !__sync_bool_compare_and_swap(&m_tail, old_tail, new_tail) );
	m_queue[new_tail] = t;

	while( !__sync_bool_compare_and_swap(&m_max_read_index, old_tail, new_tail) ) 
	{
		sched_yield();
	}
	return true;
}

template<typename T>
bool CAQueue<T>::pop(T& t)
{
	uint32_t old_head;
	uint32_t new_head;
	do {
		old_head = m_head;
		new_head = (old_head + 1) % m_capacity;
		if(new_head == m_max_read_index) {
			return false;
		}
		t = m_queue[new_head];
	} while( !__sync_bool_compare_and_swap(&m_head, old_head, new_head) );
	return true;
}

#endif

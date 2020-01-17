#ifndef JULIATLIU_LOCK_FREE_QUEUE_H
#define JULIATLIU_LOCK_FREE_QUEUE_H

#include <sched.h>
#include <atomic>
#include <stdio.h>

template<typename T>
class CAQueue {
public:
	CAQueue(uint32_t capacity)
		: m_head(0)
		, m_tail(0)
		, m_max_read_index(0)
		, m_capacity(capacity)
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


template <typename T> 
class MBQueue{ 
public: 
    MBQueue(int capacity) 
        : head_(0) 
        , tail_(0) 
        , CAPACITY(capacity) 
    {   
        memset(data_, 0, CAPACITY); 
    }   
    bool push(const T& t); 
    bool pop(T& t);
	bool empty() const { return head_.load(std::memory_order_acquire) == tail_.load(std::memory_order_acquire); }	 
private: 
    std::atomic<int> head_; 
    std::atomic<int> tail_; 
    int CAPACITY; 
    T data_[100]; 
}; 
 
template <typename T> 
bool MBQueue<T>::push(const T& t) 
{ 
    int tail = tail_.load(std::memory_order_relaxed); 
    int new_tail = (tail + 1) % CAPACITY; 
 
    if(new_tail == head_.load(std::memory_order_acquire)) 
    {   
        return false; 
    }   
 
    data_[tail] = t; 
    tail_.store(new_tail, std::memory_order_release); 
    return true; 
} 
 
template <typename T> 
bool MBQueue<T>::pop(T& t) 
{ 
    int head = head_.load(std::memory_order_relaxed); 
    if(head == tail_.load(std::memory_order_acquire)) { 
        return false; 
    }   
    t = data_[head]; 
    head_.store( (head+1) % CAPACITY, std::memory_order_release ); 
    return true; 
}


#endif

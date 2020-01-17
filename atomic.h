#ifndef JULIATLIU_ATOMIC_H
#define JULIATLIU_ATOMIC_H

#include <atomic>
#include <type_traits>
#include <iostream>
#include "default_value.h"

namespace julia {

template<typename T, std::memory_order mem_order>
struct Atomic
{
	using value_type = T;
	Atomic( double val = julia::default_value)
		 : m_value( val )
	{
		static_assert( std::is_arithmetic<T>::value, "Atomic only works on arithmetic types");
	}

	Atomic( const julia::default_value_t& )
		: m_value( (T)julia::default_value )
	{
	}

	Atomic( Atomic&& other )
		: m_value( other )
	{
	}

	Atomic( const Atomic& other )
		: m_value( other )
	{
	}

	Atomic& operator += (T value) noexcept
	{
		add( value );
		return *this;
	}

	Atomic& operator -= (T value) noexcept
	{
		sub( value );
		return *this;
	}

	Atomic& operator = ( const Atomic& other )
	{
		*this = other.get();
		return *this;
	}

	operator T() const
	{
		return get();
	}
	
	
	typename std::enable_if< std::memory_order_relaxed == mem_order, Atomic&>::type operator = (T val)
	{
		m_value.store( val, mem_order);
		return *this;
	}


private: 
	typename std::enable_if< std::memory_order_relaxed == mem_order, T>::type get() const
	{
		return m_value.load( mem_order );
	}

	template<typename Tp = T>
	void add(Tp value)
	{
		m_value.fetch_add(value, mem_order);
	}

	void add( double value )
	{
		double old = m_value.load();
		double desired = old + value;
		while( !m_value.compare_exchange_weak(old, desired) )
		{
			desired = old + value;
		}
	}

	template <typename Tp = T>
	void sub(Tp value)
	{
		m_value.fetch_sub(value, mem_order);
	}

	void sub( double value )
	{
		double old = m_value.load();
		double desired = old - value;
		while( !m_value.compare_exchange_weak(old, desired) )
		{
			desired = old - value;
		}
	}

	void sub( int value )
	{
		int old = m_value.load();
		double desired = old - value;
		while( !m_value.compare_exchange_weak(old, desired) )
		{
			desired = old - value;
		}
	}

    std::atomic<T> m_value;
};


}
#endif

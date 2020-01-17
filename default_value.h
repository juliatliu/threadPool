#ifndef JULIATLIU_DEFAULT_VALUE_H
#define JULIATLIU_DEFAULT_VALUE_H

namespace julia
{
class default_value_t
{
public:
	template<typename T>
	operator T() const { return T(); }
};

default_value_t const default_value = default_value_t();

template <typename T>
bool operator == (const julia::default_value_t& a, const T& b)
{
	return T() == b;
}

template <typename T>
bool operator != (const julia::default_value_t& a, const T& b)
{
	return T() != b;
}

}

#endif

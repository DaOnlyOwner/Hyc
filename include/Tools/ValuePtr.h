#pragma once
#include <type_traits>


template<typename T>
class ValuePtr
{
public:

	ValuePtr(){}

	ValuePtr(T* val)
		:val(val){}


	ValuePtr(std::nullptr_t) {}
	

	ValuePtr(const ValuePtr& other)
		: val(new T(other.val))
	{
	}

	template<typename U>
	ValuePtr(ValuePtr<U>&& other) noexcept
		:val(other.get())
	{
		static_assert(std::is_base_of_v<T,U>);
		other.release();
	}

	ValuePtr& operator=(std::nullptr_t)
	{
		delete val;
		return *this;
	}

	ValuePtr& operator=(const ValuePtr& other)
	{
		if (this == &other)
			return *this;
		delete val;
		val = new T(other.val);
		return *this;
	}

	//template<typename U>
	ValuePtr& operator=(ValuePtr&& other) noexcept
	{
		if (this == &other)
			return *this;
		delete val;
		val = other.val;
		other.val = nullptr;
		return *this;
	}

	T* operator ->()
	{
		return val;
	}

	const T* operator ->() const
	{
		return val;
	}

	T* get() const
	{
		return val;
	}

	T& operator*() const
	{
		return *val;
	}

	explicit operator bool() const
	{
		return val;
	}

	bool is_null() const
	{
		return val == nullptr;
	}

	T* release()
	{
		auto* tmp = val;
		val = nullptr;
		return tmp;
	}

	template<typename Derived>
	explicit operator ValuePtr<Derived>()
	{
		static_assert(std::is_base_of_v<T, Derived>);
		return reinterpret_cast<ValuePtr<Derived>>(this);
	}

	~ValuePtr()
	{
		delete val;
	}
private:
	T* val = nullptr;
};

template<typename T,typename...TArgs>
ValuePtr<T> make_value(TArgs&&... args)
{
	return ValuePtr<T>(new T(std::forward<TArgs>(args)...));
}
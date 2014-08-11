#ifndef __BUFFER_H__
#define __BUFFER_H__

#include <stdlib.h>

#include "Log.h"

template<typename T>
class Buffer
{
public:
	Buffer(size_t capacity = 1024)
	: capacity_(capacity)
	, size_(0)
	{
		if (capacity_ == 0)
		{
			data_ = nullptr;
			return;
		}

		data_ = reinterpret_cast<T*>(malloc(capacity * sizeof(T)));
		if (!data_)
		{
			THROW_SYSTEM_ERROR();
		}
	}

	~Buffer()
	{
		if (data_)
		{
			free(data_);
		}
	}

	size_t size() const
	{
		return size_;
	}

	size_t capacity() const
	{
		return capacity_;
	}

	void setCapacity(size_t capacity)
	{
		capacity_ = capacity;
		errno = 0;
		data_ = reinterpret_cast<T*>(
			realloc(data_, capacity_ * sizeof(T)));
		if (errno)
		{
			THROW_SYSTEM_ERROR();
		}
	}

	void append(const T *src, size_t size)
	{
		if (capacity_ < size_ + size)
		{
			setCapacity(capacity_ + size);
		}

		if (src >= data_ && src < data_ + capacity_)
		{
			_E("src and dst do ovelap")
		}
		memcpy(data_ + size_, src, size);

		size_ += size;
	}

	void drain(size_t n)
	{
		if (n == 0)
			return;

		if (n > size_)
		{
			THROW("not enough data");
		}

		size_t count = size_ - n;
		T *ptr1 = data_;
		T *ptr2 = data_ + n;

		while (count)
		{
			size_t count2 = std::min(n, count);

			memcpy(ptr1, ptr2, count2);
			ptr1 = ptr2;
			ptr2 += count2;
			count -= count2;
		}

		size_ -= n;
	}

	const T* data()
	{
		return data_;
	}

private:
	T* data_;
	size_t capacity_;
	size_t size_;
};

#endif /* __BUFFER_H__ */

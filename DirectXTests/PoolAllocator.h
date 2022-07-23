#pragma once

#include <stack>

template<typename T>
class PoolAllocator {
public:
	PoolAllocator(size_t capacity);
	T* get();	
	void clear();
	void free(T* ptr);
private:
	T* m_buffer;
	size_t m_capacity;
};

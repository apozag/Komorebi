#pragma once

#include <iostream>
#include <vector>
#include <utility>

#include "Core/Exceptions/Exception.h"

#define ALLOC_BUFF_MAX 512

namespace memory {

class Factory {

private:

  struct AllocatorBuffer {
    const size_t m_size = 0;
    size_t m_count = 0;
    void* m_ptr = nullptr;
    size_t m_unusedIdx = 0;
    std::vector<size_t> m_markers;    

    inline AllocatorBuffer(size_t size) : 
      m_size(size)
    {
      m_ptr = malloc(size);
      Factory::GetBuffers().push_back(this);
      m_markers.push_back(0);
    }
  };

  class MemoryException : public Exception {
  public:
    MemoryException(int line, const char* file) noexcept 
      : Exception(line, file) 
    {}

    const char* GetType() const noexcept override {
      return "Memory Exception";
    }
  };

public:

  template<class T, typename ...Args>
  static inline T* Create(Args&&... args) {
    AllocatorBuffer& buffer = GetAllocatorBuffer<T>();

    if ((buffer.m_count+1) * sizeof(T) >= buffer.m_size) {
      throw MemoryException(__LINE__, __FILE__);
    }
    
    T* ptr = ((T*)buffer.m_ptr) + buffer.m_count;
    if (buffer.m_count < buffer.m_unusedIdx) {
      ptr->~T();
    }
    new (ptr) T(std::forward<Args>(args)...);

    buffer.m_count++;

    return ptr;
  }

  static inline void SetMarker() {
    for (AllocatorBuffer* buffer : GetBuffers()) {
      buffer->m_markers.push_back(buffer->m_count);
    }
  }

  static inline void FreeToMarker() {
    for (AllocatorBuffer* buffer : GetBuffers()) {
      if (buffer->m_count > buffer->m_unusedIdx) {
        buffer->m_unusedIdx = buffer->m_count;
      }
      buffer->m_count = buffer->m_markers.back();
      if (buffer->m_markers.size() > 1) {
        buffer->m_markers.pop_back();
      }
    }
  }

private:

  template<class T>
  static inline AllocatorBuffer& GetAllocatorBuffer() {
    static AllocatorBuffer buffer(ALLOC_BUFF_MAX * sizeof(T));
    return buffer;
  }

  static std::vector<AllocatorBuffer*>& GetBuffers() {
    static std::vector<AllocatorBuffer*> buffers;
    return buffers;
  }

};

}

#define ALLOC(TYPE) memory::Factory::Allocate<TYPE>()

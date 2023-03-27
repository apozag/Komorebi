#pragma once

#include <iostream>
#include <vector>
#include <utility>

#include "Core/Exceptions/Exception.h"
#include "Core/Memory/PoolAllocator.h"

namespace memory {

class Factory {
private:

  /*class MemoryException : public Exception {
  public:
    MemoryException(int line, const char* file) noexcept 
      : Exception(line, file) 
    {}

    const char* GetType() const noexcept override {
      return "Factory buffer out of memory";
    }
  };*/

public:

  static void Init(size_t globalSize, size_t transientSize) {
    m_globalPool.Init(globalSize);
    m_transientPool.Init(transientSize);
  }

  template<class T, typename ...Args>
  static inline T* Create(Args&&... args) {    

    T* ptr = nullptr;
    
    if (m_globalMode) {
      ptr = (T*)m_globalPool.Allocate(sizeof(T));
    }
    else {
      ptr = (T*)m_transientPool.Allocate(sizeof(T));
    }

    if (ptr != nullptr) {
      new (ptr) T(std::forward<Args>(args)...);
    }

    return ptr;
  }

  template<class T>
  static inline void Destroy(T* ptr) {
    if (ptr == nullptr) {
      return;
    }
    ptr->~T();
    m_transientPool.Deallocate(ptr);
  }

  static inline void FreeAll() {
    m_transientPool.DeallocateAll();
  }

  static void SetGlobalMode(bool enabled) {
    m_globalMode = enabled;
  }

private:
  inline static bool m_globalMode = true;
  inline static MemoryPool m_globalPool;
  inline static MemoryPool m_transientPool;

};

}
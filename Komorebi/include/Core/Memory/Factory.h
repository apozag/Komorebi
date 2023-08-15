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
    m_modeStack.push_back(true);
  }

  template<class T, typename ...Args>
  static inline T* Create(Args&&... args) {    

    T* ptr = nullptr;
    
    if (IsCurrentModeGlobal()) {
      ptr = (T*)m_globalPool.Allocate(sizeof(T), alignof(T));
    }
    else {
      ptr = (T*)m_transientPool.Allocate(sizeof(T), alignof(T));
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
    if (!m_transientPool.Deallocate(ptr) && !m_globalPool.Deallocate(ptr)) {
      //TODO: [ERROR] Tried to destroy an object that is not present in any pool
    }
  }

  static inline void FreeAll() {
    m_transientPool.DeallocateAll();
  }

  static void SetGlobalMode(bool enabled) {
    m_modeStack.clear();
    m_modeStack.push_back(enabled);
  }

  static void PushGlobalMode(bool enabled) {
    m_modeStack.push_back(enabled);
  }

  static void PopGlobalMode() {
    if (m_modeStack.size() > 1) {
      m_modeStack.pop_back();
    }
  }

  static bool IsCurrentModeGlobal() {
    return m_modeStack.empty() ? true : m_modeStack[m_modeStack.size() - 1];
  }

private:
  inline static MemoryPool m_globalPool;
  inline static MemoryPool m_transientPool;
  inline static std::vector<bool> m_modeStack;
};

}
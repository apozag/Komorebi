#pragma once

namespace memory {
  class MemoryPool {
  private:
  public:
    void Init(size_t size);
    bool IsInitialized() {
      return m_pData != nullptr;
    }
    void* Allocate(size_t size);
    void Deallocate(void*);
    void DeallocateAll();
  private:
    size_t* m_pChunkUsedBytes = nullptr;
    size_t m_numChunks = 0;
    void* m_pData = nullptr;
    size_t m_totalSize;
    unsigned int m_cursorIdx = 0;
  };
}

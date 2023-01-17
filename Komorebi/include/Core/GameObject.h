#pragma once

class GameObject {
public:
  virtual void Setup() { m_initialized = true; };
  bool IsInitialized() { return m_initialized; }
private:
  bool m_initialized = false;
};
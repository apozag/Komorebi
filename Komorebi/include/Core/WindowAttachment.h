#pragma once

#include <Windows.h>

struct ID3D11DeviceContext;
struct ID3D11Device;

namespace gfx { class Graphics; }

class WindowAttachment {
  friend class Window;
  friend class gfx::Graphics;

public:
  virtual bool IsBlocking() { return false; }

protected: 
  virtual void Setup() = 0;
  virtual LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) = 0;

  ID3D11DeviceContext* m_context;
  ID3D11Device* m_device;
  HWND m_hwnd;
};

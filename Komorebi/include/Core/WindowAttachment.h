#pragma once

#include <Windows.h>

struct ID3D11DeviceContext;
struct ID3D11Device;

//struct HWND__; typedef HWND__* HWND;

class WindowAttachment {
  friend class Window;
  friend class Graphics;

protected: 
  virtual void Setup() = 0;
  virtual LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) = 0;

  ID3D11DeviceContext* m_context;
  ID3D11Device* m_device;
  HWND m_hwnd;
};

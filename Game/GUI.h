#pragma once

#include "Core/WindowAttachment.h"

class GUIAttachment : public WindowAttachment {
protected:
  virtual void Setup() override;
  virtual LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) override;
};
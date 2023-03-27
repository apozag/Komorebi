#pragma once

#include <Windows.h>
#include <sstream>
#include <optional>
#include "Core/Exceptions/Exception.h"
#include "Input/Keyboard.h"
#include "Input/Mouse.h"

class WindowAttachment;
namespace gfx {
	class Graphics;
}

class Window {
public: 
	class WindowException : public Exception
	{
		using Exception::Exception;
	public:
		static std::string TranslateErrorCode(HRESULT hr) noexcept;
	};
	class HrException : public WindowException
	{
	public:
		HrException(int line, const char* file, HRESULT hr) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorDescription() const noexcept;
	private:
		HRESULT hr;
	};
	class NoGfxException : public WindowException
	{
	public:
		using WindowException::WindowException;
		const char* GetType() const noexcept override;
	};
private: 
	class WindowClass {
	public: 
		static const char* GetName() noexcept;
		static HINSTANCE GetInstance() noexcept;
	private:
		WindowClass() noexcept;
		~WindowClass();
		WindowClass(const WindowClass&) = delete;
		WindowClass& operator=(const WindowClass&) = delete;
		static constexpr const char* wndClassName = "DirectX Example";
		static WindowClass wndClass;
		HINSTANCE hInst;
	};
public:
	Window(int width, int height, const char* name) : m_width(width), m_height(height), m_name(name) {}
	~Window();
	Window(const Window& window) = delete;
	void Init();
	static std::optional<int> ProcessMessages();
	void Attach(WindowAttachment* attachment);
private:
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT mg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT mg, WPARAM wParam, LPARAM lParam);
	LRESULT HandleMsg(HWND hWnd, UINT mg, WPARAM wParam, LPARAM lParam);
public: 
	Keyboard m_keyboard;
	Mouse m_mouse;
	gfx::Graphics* m_gfx;
private: 
	std::vector<WindowAttachment*> m_attachments;
	int m_width, m_height;
	std::string m_name;
	HWND m_hWnd;
};

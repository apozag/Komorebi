#define NOMINMAX

#include "Core/Window.h"
#include "Graphics/Graphics.h"
#include "Core/Exceptions/WindowsThrowMacros.h"

#include "Core/WindowAttachment.h"
#include "Core/Memory/Factory.h"

// Window Exception Stuff
std::string Window::WindowException::TranslateErrorCode(HRESULT hr) noexcept
{
	char* pMsgBuf = nullptr;
	// windows will allocate memory for err string and make our pointer point to it
	const DWORD nMsgLen = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPSTR>(&pMsgBuf), 0, nullptr
	);
	// 0 string length returned indicates a failure
	if (nMsgLen == 0)
	{
		return "Unidentified error code";
	}
	// copy error string from windows-allocated buffer to std::string
	std::string errorString = pMsgBuf;
	// free windows buffer
	LocalFree(pMsgBuf);
	return errorString;
}


Window::HrException::HrException(int line, const char* file, HRESULT hr) noexcept
	:
	WindowException(line, file),
	hr(hr)
{}

const char* Window::HrException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
		<< "[Description] " << GetErrorDescription() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Window::HrException::GetType() const noexcept
{
	return "Chili Window Exception";
}

HRESULT Window::HrException::GetErrorCode() const noexcept
{
	return hr;
}

std::string Window::HrException::GetErrorDescription() const noexcept
{
	return WindowException::TranslateErrorCode(hr);
}


const char* Window::NoGfxException::GetType() const noexcept
{
	return "Chili Window Exception [No Graphics]";
}


Window::WindowClass Window::WindowClass::wndClass;

Window::WindowClass::WindowClass() noexcept :
	hInst(GetModuleHandle(nullptr))
{
	// register winbdow class
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.lpfnWndProc = HandleMsgSetup;
	wc.style = CS_OWNDC;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetInstance();
	wc.hIcon = nullptr;
	wc.hCursor = nullptr;
	wc.lpszMenuName = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszClassName = GetName();
	wc.hIconSm = nullptr;
	RegisterClassEx(&wc);
}

Window::WindowClass::~WindowClass() {
	UnregisterClass(wndClassName, GetInstance());
}
const char* Window::WindowClass::GetName() noexcept {
	return wndClassName;
}

HINSTANCE Window::WindowClass::GetInstance() noexcept {
	return wndClass.hInst;
}

Window::~Window() {
	DestroyWindow(m_hWnd);
}

std::optional<int> Window::ProcessMessages() {
	MSG msg;
	BOOL gResult;
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {

		if (msg.message == WM_QUIT) {
			return msg.wParam;
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);

	}
	return {};
}

void Window::Init() {
	RECT wr;
	wr.left = 100;
	wr.right = m_width + wr.left;
	wr.top = 100;
	wr.bottom = m_height + wr.top;

	if (FAILED(AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE))) {
		throw WND_LAST_EXCEPT();
	}

	m_hWnd = CreateWindow(
		WindowClass::GetName(),
		m_name.c_str(),
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT, m_width, m_height,
		nullptr, nullptr, WindowClass::GetInstance(), this

	);

	if (m_hWnd == nullptr) {
		throw WND_LAST_EXCEPT();
	}

	ShowWindow(m_hWnd, SW_SHOWDEFAULT);

	m_gfx = memory::Factory::Create<gfx::Graphics>(m_hWnd, m_width, m_height);
	m_gfx->Init();
}

LRESULT CALLBACK Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (msg == WM_NCCREATE) {
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));
		return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
LRESULT CALLBACK Window::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}
LRESULT Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	bool blocked = false;

	for (WindowAttachment* attachment : m_attachments) {
		attachment->WndProc(hWnd, msg, wParam, lParam);
		if (attachment->IsBlocking()) {
			return DefWindowProc(hWnd, msg, wParam, lParam);
			blocked = true;
		}
	}

	switch (msg) {
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;

	case WM_KILLFOCUS:
		m_keyboard.ClearState();
		break;
	}

	if (!blocked) {
		switch (msg) {
			/***KEYBOARD MESSAGES***/
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
			if (wParam == 27)
				PostQuitMessage(0);
			else if (!(lParam & 0x40000000) || m_keyboard.AutorepeatIsEnabled())
				m_keyboard.OnKeyPressed(static_cast<unsigned char>(wParam));
			break;
		case WM_KEYUP:
		case WM_SYSKEYUP:
			m_keyboard.OnKeyReleased(static_cast<unsigned char>(wParam));
			break;
		case WM_CHAR:
			m_keyboard.OnChar(static_cast<unsigned char>(wParam));
			break;
			/* END KEYBOARD MESSAGES*/

			/*MOUSE MESSAGES*/
		case WM_RBUTTONDOWN:
		{
			const POINTS p = MAKEPOINTS(lParam);
			m_mouse.OnLeftPressed(p.x, p.y);
			break;
		}
		case WM_RBUTTONUP:
		{
			const POINTS p = MAKEPOINTS(lParam);
			m_mouse.OnRightReleased(p.x, p.y);
			break;
		}
		case WM_LBUTTONDOWN:
		{
			const POINTS p = MAKEPOINTS(lParam);
			m_mouse.OnLeftPressed(p.x, p.y);
			break;
		}
		case WM_LBUTTONUP:
		{
			const POINTS p = MAKEPOINTS(lParam);
			m_mouse.OnLeftReleased(p.x, p.y);
			break;
		}
		case WM_MOUSEWHEEL:
		{
			const POINTS p = MAKEPOINTS(lParam);
			if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)
				m_mouse.OnWheelUp(p.x, p.y);
			else if (GET_WHEEL_DELTA_WPARAM(wParam) < 0)
				m_mouse.OnWheelDown(p.x, p.y);
			break;
		}
		case WM_MOUSEMOVE:
		{
			const POINTS p = MAKEPOINTS(lParam);
			m_mouse.OnMouseMove(p.x, p.y);
			break;
		}
		/*END MOUSE MESSAGES*/
		}
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

void Window::Attach(WindowAttachment* attachment) {
	attachment->m_hwnd = m_hWnd;
	m_gfx->Attach(attachment);
	attachment->Setup();
	m_attachments.push_back(attachment);
}

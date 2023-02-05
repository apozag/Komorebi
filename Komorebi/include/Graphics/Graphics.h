#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <DirectXMath.h>
#include <vector>
#include "Core/Exceptions/Exception.h"
#include "Graphics/DxgiInfoManager.h"

class Bindable;
class RenderTarget;
class Viewport;
class WindowAttachment;

class Graphics {
	friend class Bindable;	
public:
	class HrException : public Exception
	{
	public:
		HrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs = {}) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
		std::string GetErrorDescription() const noexcept;
		std::string GetErrorInfo() const noexcept;
	private:
		HRESULT hr;
		std::string info;
	};
	class InfoException : public Exception
	{
	public:
		InfoException(int line, const char* file, std::vector<std::string> infoMsgs) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		std::string GetErrorInfo() const noexcept;
	private:
		std::string info;
	};
	class DeviceRemovedException : public HrException
	{
		using HrException::HrException;
	public:
		const char* GetType() const noexcept override;
	private:
		std::string reason;
	};
public:
	Graphics(HWND hWnd, int width, int height);
	Graphics& operator=(const Graphics&) = delete;
	Graphics(const Graphics&) = delete;
	~Graphics();
	void SwapBuffers();
	void ClearBuffer(float r, float g, float b) noexcept;
	void Init();
	void Draw(unsigned int count);
	void DrawIndexed(unsigned int count) const;
	RenderTarget* GetDefaultRenderTarget() { 
		return m_target; 
	}
	void Attach(WindowAttachment* attachment);
private:
	Microsoft::WRL::ComPtr<ID3D11Device> m_device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context;
	Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain;
	RenderTarget* m_target;
	Viewport* m_viewport;
	int m_viewportWidth, m_viewportHeight;
#ifndef NDEBUG
	DxgiInfoManager infoManager;
#endif
};

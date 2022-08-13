#include "Graphics.h"
#include <sstream>
#include <d3dcompiler.h>
#include "GraphicsThrowMacros.h"
#include "dxerr.h"
#include "RenderTarget.h"
#include "Viewport.h"

namespace wrl = Microsoft::WRL;

// Graphics exception stuff
Graphics::HrException::HrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs) noexcept
	:
	Exception(line, file),
	hr(hr)
{
	// join all info messages with newlines into single string
	for (const auto& m : infoMsgs)
	{
		info += m;
		info.push_back('\n');
	}
	// remove final newline if exists
	if (!info.empty())
	{
		info.pop_back();
	}
}

const char* Graphics::HrException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
		<< "[Error String] " << GetErrorString() << std::endl
		<< "[Description] " << GetErrorDescription() << std::endl;
	if (!info.empty())
	{
		oss << "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	}
	oss << GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Graphics::HrException::GetType() const noexcept
{
	return "Chili Graphics Exception";
}

HRESULT Graphics::HrException::GetErrorCode() const noexcept
{
	return hr;
}

std::string Graphics::HrException::GetErrorString() const noexcept
{
	return DXGetErrorString(hr);
}

std::string Graphics::HrException::GetErrorDescription() const noexcept
{
	char buf[512];
	DXGetErrorDescription(hr, buf, sizeof(buf));
	return buf;
}

std::string Graphics::HrException::GetErrorInfo() const noexcept
{
	return info;
}


const char* Graphics::DeviceRemovedException::GetType() const noexcept
{
	return "Chili Graphics Exception [Device Removed] (DXGI_ERROR_DEVICE_REMOVED)";
}
Graphics::InfoException::InfoException(int line, const char* file, std::vector<std::string> infoMsgs) noexcept
	:
	Exception(line, file)
{
	// join all info messages with newlines into single string
	for (const auto& m : infoMsgs)
	{
		info += m;
		info.push_back('\n');
	}
	// remove final newline if exists
	if (!info.empty())
	{
		info.pop_back();
	}
}


const char* Graphics::InfoException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	oss << GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Graphics::InfoException::GetType() const noexcept
{
	return "Chili Graphics Info Exception";
}

std::string Graphics::InfoException::GetErrorInfo() const noexcept
{
	return info;
}



Graphics::Graphics(HWND hWnd, int width, int height) : m_viewportWidth(width), m_viewportHeight(height)
{
	RECT rect;
	D3D_FEATURE_LEVEL DX11 = D3D_FEATURE_LEVEL_11_0;
	DXGI_SWAP_CHAIN_DESC sdesc;
	ZeroMemory(&sdesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	sdesc.BufferDesc.Width = width;
	sdesc.BufferDesc.Height = height;
	sdesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sdesc.BufferDesc.RefreshRate.Numerator = 0;
	sdesc.BufferDesc.RefreshRate.Denominator = 0;
	sdesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sdesc.SampleDesc.Count = 1;
	sdesc.SampleDesc.Quality = 0;
	sdesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sdesc.BufferCount = 1;
	sdesc.OutputWindow = hWnd;
	sdesc.Windowed = TRUE;
	sdesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sdesc.Flags = 0;

	UINT swapCreateFlags = 0u;
#ifndef NDEBUG
	swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	HRESULT hr;

	GFX_THROW_INFO(D3D11CreateDeviceAndSwapChain(
		nullptr, 
		D3D_DRIVER_TYPE_HARDWARE, 
		nullptr, 
		swapCreateFlags,
		nullptr, 
		0, 
		D3D11_SDK_VERSION, 
		&sdesc, 
		&m_swapChain, 
		&m_device, 
		nullptr, 
		&m_context));

	// Back buffer creation
	/*
	wrl::ComPtr<ID3D11Texture2D> pBackBuffer;
	GFX_THROW_INFO(m_swapChain->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer));		
	pBackBuffer->Release();
	*/
}

Graphics::~Graphics()
{
	if (m_device)
		m_device->Release();
	if (m_swapChain)
		m_swapChain->Release();
	if (m_context)
		m_context->Release();
}

void Graphics::SwapBuffers()
{
	HRESULT hr;
#ifndef NDEBUG
	infoManager.Set();
#endif
	if (FAILED(hr = m_swapChain->Present(1u, 0u)))
	{
		if (hr == DXGI_ERROR_DEVICE_REMOVED)
		{
			throw GFX_DEVICE_REMOVED_EXCEPT(m_device->GetDeviceRemovedReason());
		}
		else
		{
			throw GFX_EXCEPT(hr);
		}
	}
}

void Graphics::ClearBuffer(float r, float g, float b) noexcept
{
	m_target->Clear( r, g, b);
}

void Graphics::Init()
{
	m_target = new RenderTarget(m_swapChain);
	m_target->SetAsRenderTarget();

	m_viewport = new Viewport(0,0, m_viewportWidth, m_viewportHeight);

	m_target->Bind();
	m_viewport->Bind();

	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Graphics::Draw(unsigned int count) {	
	m_context->Draw(count, 0);
}
void Graphics::DrawIndexed(unsigned int count) const {
	m_context->DrawIndexed((UINT)count, 0u, 0u);
}
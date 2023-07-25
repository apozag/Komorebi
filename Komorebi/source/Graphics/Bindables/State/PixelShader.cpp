#include "d3dcompiler.h"
//#include <D3d11shader.h>

#include "Core/Reflection/ReflectionImplMacros.h"
#include "Core/Util/WideStringUtility.h"
#include "Graphics/Bindables/State/PixelShader.h"
#include "Graphics/GraphicsThrowMacros.h"

namespace gfx {

	PixelShader::~PixelShader() {
		//m_pixelShader->Release();
		//m_reflection->Release();
	}

	void PixelShader::Setup() {

		INFOMAN

		Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
		GFX_THROW_INFO(D3DReadFileToBlob(WideStringUtility::charToLPWSTR(m_filename.c_str()).c_str(), &pBlob));

		GFX_THROW_INFO(D3DReflect(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)m_reflection.GetAddressOf()));

		GFX_THROW_INFO(GetDevice()->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, m_pixelShader.GetAddressOf()));
	}

	void PixelShader::Bind() const {
		GetContext()->PSSetShader(m_pixelShader.Get(), nullptr, 0u);
	}

	REFLECT_STRUCT_BEGIN(PixelShader, StateBindable)
		REFLECT_STRUCT_MEMBER(m_filename)
		REFLECT_STRUCT_END(PixelShader)

}

IMPLEMENT_REFLECTION_POINTER_NAMESPACE(gfx, PixelShader)

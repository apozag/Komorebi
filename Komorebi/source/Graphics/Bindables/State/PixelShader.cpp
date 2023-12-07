#include "Graphics/ShaderInclude.h"
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
		GFX_THROW_INFO(D3DReadFileToBlob(WideStringUtility::charToLPWSTR(m_filename.c_str()).c_str(), pBlob.GetAddressOf()));

		GFX_THROW_INFO(D3DReflect(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)m_reflection.ReleaseAndGetAddressOf()));

		GFX_THROW_INFO(GetDevice()->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, m_pixelShader.ReleaseAndGetAddressOf()));
	}

	void PixelShader::Bind() const {
		GetContext()->PSSetShader(m_pixelShader.Get(), nullptr, 0u);
	}

	void PixelShader::HotReload() {

		INFOMAN

		UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
		flags |= D3DCOMPILE_DEBUG;
#endif
		LPCSTR profile = (GetDevice()->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0) ? "ps_5_0" : "ps_4_0";
		ID3DBlob* pBlob = nullptr;
		ID3DBlob* pErrorBlob = nullptr;
		std::string srcFilename = m_filename.substr(0, m_filename.find_last_of('.') + 1) + "hlsl";
		std::string srcDirectory = m_filename.substr(0, m_filename.find_last_of('/'));
		ShaderInclude includeObj(srcDirectory.c_str(), "Komorebi/assets/shaders/include");
		hr = D3DCompileFromFile(WideStringUtility::charToLPWSTR(srcFilename.c_str()).c_str(), NULL, &includeObj,
			"main", profile, flags, 0, &pBlob, &pErrorBlob);

		if (FAILED(hr)) {
			if (pErrorBlob) {
				OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
				pErrorBlob->Release();
			}
			if (pBlob) {
				pBlob->Release();
			}
		}
		else {
			GFX_THROW_INFO(D3DReflect(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)m_reflection.ReleaseAndGetAddressOf()));

			GFX_THROW_INFO(GetDevice()->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, m_pixelShader.ReleaseAndGetAddressOf()));
		}
	}	

	REFLECT_STRUCT_BEGIN(PixelShader, StateBindable)
		REFLECT_STRUCT_MEMBER(m_filename)
		REFLECT_STRUCT_END(PixelShader)

}

IMPLEMENT_REFLECTION_POINTER_NAMESPACE(gfx, PixelShader)

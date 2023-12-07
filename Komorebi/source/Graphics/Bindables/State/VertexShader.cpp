#include "Graphics/Bindables/State/VertexShader.h"
#include "Graphics/ShaderInclude.h"
#include "Core/Util/WideStringUtility.h"
#include "Graphics/GraphicsThrowMacros.h"
#include "Core/Reflection/ReflectionImplMacros.h"

namespace gfx {  
  
	VertexShader::~VertexShader() {
		//m_vertexShader->Release();
		//m_bytecodeBlob->Release();
	}

	void VertexShader::Setup() {

		INFOMAN
		GFX_THROW_INFO(D3DReadFileToBlob(WideStringUtility::charToLPWSTR(m_filename.c_str()).c_str(), m_bytecodeBlob.ReleaseAndGetAddressOf()));

		GFX_THROW_INFO(D3DReflect(m_bytecodeBlob->GetBufferPointer(), m_bytecodeBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)m_reflection.ReleaseAndGetAddressOf()));

		GFX_THROW_INFO(GetDevice()->CreateVertexShader(m_bytecodeBlob->GetBufferPointer(), m_bytecodeBlob->GetBufferSize(), nullptr, m_vertexShader.ReleaseAndGetAddressOf()));
	}

	void VertexShader::Bind() const {
		GetContext()->VSSetShader(m_vertexShader.Get(), nullptr, 0u);
	}
	void VertexShader::Unbind() const {
	}
	void VertexShader::Update() {
	}

	ID3DBlob* VertexShader::GetBytecode() const {
		return m_bytecodeBlob.Get();
	}

	void VertexShader::HotReload() {
		INFOMAN

		UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
		flags |= D3DCOMPILE_DEBUG;
#endif
		LPCSTR profile = (GetDevice()->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0) ? "vs_5_0" : "vs_4_0";
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

			if (pBlob)
				pBlob->Release();
		}
		else {
			GFX_THROW_INFO(D3DReflect(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)m_reflection.ReleaseAndGetAddressOf()));

			GFX_THROW_INFO(GetDevice()->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, m_vertexShader.ReleaseAndGetAddressOf()));
		}
	}

	REFLECT_STRUCT_BEGIN(VertexShader, StateBindable)
		REFLECT_STRUCT_MEMBER(m_filename)
		REFLECT_STRUCT_END(VertexShader)
}

IMPLEMENT_REFLECTION_POINTER_NAMESPACE(gfx, VertexShader)

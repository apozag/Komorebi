#include "d3dcompiler.h"

#include "Core/Reflection/ReflectionImplMacros.h"
#include "Core/Util/WideStringUtility.h"
#include "Graphics/Bindables/State/ComputeShader.h"
#include "Graphics/GraphicsThrowMacros.h"

namespace gfx {

	void ComputeShader::Setup() {

		INFOMAN

			Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
		GFX_THROW_INFO(D3DReadFileToBlob(WideStringUtility::charToLPWSTR(m_filename.c_str()).c_str(), &pBlob));		

		GFX_THROW_INFO(GetDevice()->CreateComputeShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, m_computeShader.GetAddressOf()));
	}

	void ComputeShader::Bind() const {
		GetContext()->CSSetShader(m_computeShader.Get(), nullptr, 0u);
	}

	void ComputeShader::Dispatch(unsigned int x, unsigned int y, unsigned int z) {
		Bind();
		GetContext()->Dispatch(x, y, z);
		Unbind();
	}

	//REFLECT_STRUCT_BEGIN(ComputeShader, StateBindable)
	//	REFLECT_STRUCT_MEMBER(m_filename)
	//	REFLECT_STRUCT_END(ComputeShader)

}

//IMPLEMENT_REFLECTION_POINTER_NAMESPACE(gfx, ComputeShader)

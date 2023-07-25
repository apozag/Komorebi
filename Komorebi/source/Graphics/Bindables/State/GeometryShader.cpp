#include "Graphics\Bindables\State\GeometryShader.h"

#include "d3dcompiler.h"
#include <D3d11shader.h>

#include "Core/Reflection/ReflectionImplMacros.h"
#include "Core/Util/WideStringUtility.h"
#include "Graphics/GraphicsThrowMacros.h"

namespace gfx {

	void GeometryShader::Setup() {

		INFOMAN

		Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
		GFX_THROW_INFO(D3DReadFileToBlob(WideStringUtility::charToLPWSTR(m_filename.c_str()).c_str(), &pBlob));

		GFX_THROW_INFO(GetDevice()->CreateGeometryShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, m_geometryShader.GetAddressOf()));
	}

	void GeometryShader::Bind() const {
		GetContext()->GSSetShader(m_geometryShader.Get(), nullptr, 0u);
	}

	void GeometryShader::Unbind() const {
		GetContext()->GSSetShader(nullptr, nullptr, 0u);
	}

	REFLECT_STRUCT_BEGIN(GeometryShader, StateBindable)
	REFLECT_STRUCT_MEMBER(m_filename)
	REFLECT_STRUCT_END(GeometryShader)

}

IMPLEMENT_REFLECTION_POINTER_NAMESPACE(gfx, GeometryShader)

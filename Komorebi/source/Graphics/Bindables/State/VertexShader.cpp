#include <d3dcompiler.h>
#include "Graphics/Bindables/State/VertexShader.h"
#include "Core/Util/WideStringUtility.h"
#include "Graphics/GraphicsThrowMacros.h"

VertexShader::~VertexShader() {
	m_vertexShader->Release();
	m_bytecodeBlob->Release();
}

void VertexShader::Setup() {

	INFOMAN;
	GFX_THROW_INFO(D3DReadFileToBlob(WideStringUtility::charToLPWSTR(m_filename.c_str()).c_str(), &m_bytecodeBlob));

	GFX_THROW_INFO(D3DReflect(m_bytecodeBlob->GetBufferPointer(), m_bytecodeBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)m_reflection.GetAddressOf()));

	GFX_THROW_INFO(GetDevice()->CreateVertexShader(m_bytecodeBlob->GetBufferPointer(), m_bytecodeBlob->GetBufferSize(), nullptr, m_vertexShader.GetAddressOf()));
}

void VertexShader::Bind( ) const {
	GetContext ()->VSSetShader(m_vertexShader.Get(), nullptr, 0u);
}
void VertexShader::Unbind( ) const {
}
void VertexShader::Update( ) {
}

ID3DBlob* VertexShader::GetBytecode() const {
	return m_bytecodeBlob.Get();
}

REFLECT_STRUCT_BEGIN(VertexShader, StateBindable)
REFLECT_STRUCT_MEMBER(m_filename)
REFLECT_STRUCT_END(VertexShader)

IMPLEMENT_REFLECTION_POINTER(VertexShader)

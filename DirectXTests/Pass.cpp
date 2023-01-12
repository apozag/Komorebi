#include "Pass.h"

#include "StateBindable.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "InputLayout.h"
#include "RasterizerState.h"

unsigned char Pass::static_idx = 0;

Pass::Pass(VertexShader* vs, PixelShader* ps, unsigned int layer, bool skinned) : m_vertexShader(vs), m_pixelShader(ps), m_layer(layer), m_skinned(skinned), m_idx(static_idx++) {
	m_vertexShader->Setup();
	m_pixelShader->Setup();
}

Pass::Pass(const char* vsFilename, const char* psFilename, unsigned int layer, bool skinned) : Pass(new VertexShader(vsFilename), new PixelShader(psFilename), layer, skinned) {}

void Pass::Setup() {
	if (m_skinned) {
		const D3D11_INPUT_ELEMENT_DESC ied[] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,	0, 0,							 D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"BONES",	 0, DXGI_FORMAT_R32G32B32A32_UINT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"WEIGHTS",	 0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};
		AddBindable(new InputLayout(ied, 6, *m_vertexShader));
	}
	else {
		const D3D11_INPUT_ELEMENT_DESC ied[] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,							  D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,	 0,	D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};
		AddBindable(new InputLayout(ied, 4, *m_vertexShader));
	}
	//AddBindable(new RasterizerState());
}

void Pass::AddBindable(StateBindable* bind){
	m_binds.push_back(bind);
}

void Pass::Bind( ) {
	m_vertexShader->Bind();
	m_pixelShader->Bind();
	for (Bindable* bind : m_binds) {
		bind->Bind ();
	}
}
void Pass::Unbind( ) {
	m_vertexShader->Unbind();
	m_pixelShader->Unbind();
	for (Bindable* bind : m_binds) {
		bind->Unbind ();
	}
}

REFLECT_STRUCT_BASE_BEGIN(Pass)
REFLECT_STRUCT_MEMBER(m_layer)
REFLECT_STRUCT_MEMBER(m_pixelShader)
REFLECT_STRUCT_MEMBER(m_vertexShader)
REFLECT_STRUCT_MEMBER(m_binds)
REFLECT_STRUCT_MEMBER(m_skinned)
REFLECT_STRUCT_END(Pass)

IMPLEMENT_REFLECTION_POINTER(Pass)
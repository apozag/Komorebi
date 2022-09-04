#include "Pass.h"
#include "InputLayout.h"
#include "Rasterizer.h"
#include "Sampler.h"
#include "RenderTarget.h"

unsigned char Pass::static_idx = 0;

Pass::Pass( const char* vsFilename, const char* psFilename, unsigned int layer, bool skinned) : layer(layer) {
	VertexShader* vs = new VertexShader ( vsFilename);
	AddBindable(vs);
	if (skinned) {
		const D3D11_INPUT_ELEMENT_DESC ied[] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,	0, 0,							 D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"BONES",	 0, DXGI_FORMAT_R32G32B32A32_UINT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"WEIGHTS",	 0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};
		AddBindable(new InputLayout ( ied, 6, *vs));
	}
	else {
		const D3D11_INPUT_ELEMENT_DESC ied[] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,							  D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,	 0,	D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};
		AddBindable(new InputLayout ( ied, 4, *vs));
	}
	AddBindable(new PixelShader ( psFilename));
	AddBindable(new Rasterizer ());
	//AddBindable(new Sampler ( D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP, 0));
	m_slot = static_idx++;
}
void Pass::AddBindable(StateBindable* bind){
	m_binds.push_back(bind);
}

void Pass::Bind( ) {
	for (Bindable* bind : m_binds) {
		bind->Bind ();
	}
}
void Pass::Unbind( ) {
	for (Bindable* bind : m_binds) {
		bind->Unbind ();
	}
}
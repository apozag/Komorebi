
#include <d3d11.h>

#include "Graphics/Pass.h"

#include "Core/Reflection/ReflectionImplMacros.h"

#include "Core\Memory\Factory.h"
#include "Graphics/Bindables/State/StateBindable.h"
#include "Graphics/Bindables/State/VertexShader.h"
#include "Graphics/Bindables/State/PixelShader.h"
#include "Graphics/Bindables/State/InputLayout.h"
#include "Graphics/Bindables/State/RasterizerState.h"

namespace gfx {

	unsigned char Pass::static_idx = 0;

	Pass::Pass(VertexShader* vs, PixelShader* ps, unsigned int layer, bool skinned) : m_vertexShader(vs), m_pixelShader(ps), m_layer(layer), m_skinned(skinned), m_idx(static_idx++) {}

	Pass::Pass(const char* vsFilename, const char* psFilename, unsigned int layer, bool skinned) : m_VSFilename(vsFilename), m_PSFilename(psFilename), m_layer(layer), m_skinned(skinned), m_idx(static_idx++) {}

	void Pass::Setup() {
		m_vertexShader = memory::Factory::Create<VertexShader>(m_VSFilename.c_str());
		m_pixelShader = memory::Factory::Create<PixelShader>(m_PSFilename.c_str());
		m_vertexShader->Setup();
		m_pixelShader->Setup();
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
			AddBindable(memory::Factory::Create<InputLayout>(ied, 6, *m_vertexShader));
		}
		else {
			const D3D11_INPUT_ELEMENT_DESC ied[] =
			{
				{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,							  D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,	 0,	D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
			};
			AddBindable(memory::Factory::Create<InputLayout>(ied, 4, *m_vertexShader));
		}
	}

	Pass::~Pass() {
		/*delete(m_pixelShader);
		delete(m_vertexShader);
		for (StateBindable* bind : m_binds) {
			delete(bind);
		}*/
	}

	void Pass::AddBindable(StateBindable* bind) {
		m_binds.push_back(bind);
	}

	void Pass::Bind() const {
		if (!m_enabled) return;

		m_vertexShader->Bind();
		m_pixelShader->Bind();
		for (Bindable* bind : m_binds) {
			bind->Bind();
		}
	}
	void Pass::Unbind() const {
		if (!m_enabled) return;

		m_vertexShader->Unbind();
		m_pixelShader->Unbind();
		for (Bindable* bind : m_binds) {
			bind->Unbind();
		}
	}

	REFLECT_STRUCT_BASE_BEGIN(Pass)
		REFLECT_STRUCT_MEMBER(m_enabled)
		REFLECT_STRUCT_MEMBER(m_layer)
		REFLECT_STRUCT_MEMBER(m_PSFilename)
		REFLECT_STRUCT_MEMBER(m_VSFilename)
		REFLECT_STRUCT_MEMBER(m_binds)
		REFLECT_STRUCT_MEMBER(m_skinned)
		REFLECT_STRUCT_MEMBER(m_ignoreFrustumCulling)
		REFLECT_STRUCT_END(Pass)
}

IMPLEMENT_REFLECTION_POINTER_NAMESPACE(gfx, Pass)
IMPLEMENT_REFLECTION_VECTOR(gfx::Pass*)
IMPLEMENT_REFLECTION_VECTOR(OWNED_PTR(gfx::Pass))

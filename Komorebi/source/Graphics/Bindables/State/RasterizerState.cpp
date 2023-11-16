
#include <d3d11.h>
#include "Graphics/GraphicsThrowMacros.h"
#include "Graphics/Bindables/State/RasterizerState.h"

#include "Core/Reflection/ReflectionImplMacros.h"

#define DEPTH_BIAS_D32_FLOAT(d) (d/(1/pow(2,23)))

namespace gfx {

	RasterizerState::~RasterizerState() {
		//m_rasterizer->Release();
	}

	void RasterizerState::Setup() {

		INFOMAN

		D3D11_RASTERIZER_DESC desc = {};
		desc.FillMode = m_wireframe ? D3D11_FILL_WIREFRAME : D3D11_FILL_SOLID;
		switch (m_cullMode) {
		case NONE:
			desc.CullMode = D3D11_CULL_NONE;
			break;
		case FRONT:
			desc.CullMode = D3D11_CULL_FRONT;
			break;
		case BACK:
			desc.CullMode = D3D11_CULL_BACK;
			break;
		}
		desc.FrontCounterClockwise = false;
		desc.DepthBias = 0;// DEPTH_BIAS_D32_FLOAT(-0.00001);
		desc.DepthBiasClamp = 0;//-0.001;
		desc.SlopeScaledDepthBias = 0;// 1.f;
		desc.DepthClipEnable = true;
		desc.MultisampleEnable = true;
		desc.AntialiasedLineEnable = true;

		GFX_THROW_INFO(GetDevice()->CreateRasterizerState(&desc, &m_rasterizer));
	}

	void RasterizerState::Bind() const {
		GetContext()->RSSetState(m_rasterizer.Get());
	}
	void RasterizerState::Unbind() const {
		GetDefault()->Bind();
	}

	RasterizerState* RasterizerState::GetDefault() {
		static RasterizerState* rs = nullptr;
		if (rs == nullptr) {
			memory::Factory::PushGlobalMode(true);
			rs = memory::Factory::Create<RasterizerState>();
			memory::Factory::PopGlobalMode();
		}
		return rs;
	}	

	REFLECT_STRUCT_BEGIN(RasterizerState, StateBindable)
	REFLECT_STRUCT_MEMBER(m_cullMode)
	REFLECT_STRUCT_MEMBER(m_wireframe)
	REFLECT_STRUCT_END(RasterizerState)

}

typedef gfx::RasterizerState::CullMode CullModeType;
REFLECT_ENUM_BEGIN(CullModeType)
REFLECT_ENUM_VALUE(NONE)
REFLECT_ENUM_VALUE(FRONT)
REFLECT_ENUM_VALUE(BACK)
REFLECT_ENUM_END(CullModeType)

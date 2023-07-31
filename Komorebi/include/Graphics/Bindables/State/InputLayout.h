#pragma once

#include "Graphics/Bindables/State/StateBindable.h"
#include "Graphics/Bindables/State/VertexShader.h"

struct D3D11_INPUT_ELEMENT_DESC;
struct ID3D11InputLayout;

namespace gfx {

	class InputLayout : public StateBindable {
	public:
		InputLayout() {}
		InputLayout(const D3D11_INPUT_ELEMENT_DESC* ied, unsigned int numElements, const gfx::VertexShader& vs);
		~InputLayout();
		void Bind()const override;
		void Unbind()const override;

		REFLECT_HIDE()

	private:
		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
	};
}

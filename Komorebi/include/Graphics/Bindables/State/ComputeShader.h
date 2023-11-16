#pragma once

#include <D3d11shader.h>

#pragma comment(lib, "dxguid.lib")

#include "Graphics/Bindables/State/StateBindable.h"

struct ID3D11ComputeShader;
//struct ID3D11ShaderReflection;

namespace gfx {

	class ComputeShader : public StateBindable {
	public:
		ComputeShader() {}
		ComputeShader(const char* filename) : m_filename(filename) {}		

		void Setup() override;

		void Bind()const override;
		void Unbind()const override {}
		void Update() {}		

		void Dispatch(unsigned int x, unsigned int y, unsigned int z);

		//REFLECT()

	private:

		Microsoft::WRL::ComPtr<ID3D11ComputeShader> m_computeShader;

		std::string m_filename;
	};
}

//DECLARE_REFLECTION_POINTER(gfx::PixelShader)

#pragma once

#include "Graphics/Bindables/Resource/ResourceBindable.h"
#include "Graphics/GraphicsThrowMacros.h"
#include <d3d11shader.h>

namespace gfx {

	class ReflectedConstantBuffer : public ResourceBindable {
	public:
		class CBufferException : public Exception {
		public:
			CBufferException(int line, const char* file) noexcept;
			const char* GetType() const noexcept override;
		};
		struct ConstantBufferVariable {
			D3D11_SHADER_VARIABLE_DESC desc;
			D3D11_SHADER_TYPE_DESC typeDesc;
		};

	public:

		ReflectedConstantBuffer() {}
		ReflectedConstantBuffer(std::vector<ConstantBufferVariable> variables, unsigned int slot);

		~ReflectedConstantBuffer() {
			free(m_buffer);
		}

		virtual void Bind() const override = 0;
		virtual void Unbind() const override = 0;

		const std::vector<ConstantBufferVariable>& GetVariables() const { return m_variables; }

		bool HasFloat(const char* name);
		bool HasVector4(const char* name);

		float GetFloat(const char* name);
		void GetVector4(const char* name, float* values);

		bool SetFloat(const char* name, float value);
		bool SetVector4(const char* name, float* data);
		bool SetMat4(const char* name, float* data);

		void Update();

	private:
		bool SetVariable(D3D_SHADER_VARIABLE_CLASS varClass, const char* name, void* data, size_t size);
		void* GetVariable(D3D_SHADER_VARIABLE_CLASS varClass, const char* name);
	protected:
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer;
		float* m_buffer;
		std::vector<ConstantBufferVariable> m_variables;		
		size_t m_size;
		bool m_dynamic;
		bool m_dirty = true;
	};

	class ReflectedVertexConstantBuffer : public ReflectedConstantBuffer {
	public:
		ReflectedVertexConstantBuffer() {}
		ReflectedVertexConstantBuffer(std::vector<ConstantBufferVariable> variables, unsigned int slot) : ReflectedConstantBuffer(variables, slot) {}
		void Bind() const override {
			GetContext()->VSSetConstantBuffers(m_slot, 1u, m_constantBuffer.GetAddressOf());
		}
		void Unbind() const override {}

		REFLECT_HIDE()
	};

	class ReflectedPixelConstantBuffer : public ReflectedConstantBuffer {
	public:
		ReflectedPixelConstantBuffer() {}
		ReflectedPixelConstantBuffer(std::vector<ConstantBufferVariable> variables, unsigned int slot) : ReflectedConstantBuffer(variables, slot) {}
		void Bind() const override {
			GetContext()->PSSetConstantBuffers(m_slot, 1u, m_constantBuffer.GetAddressOf());
		}
		void Unbind() const override {}

		REFLECT_HIDE()
	};
}

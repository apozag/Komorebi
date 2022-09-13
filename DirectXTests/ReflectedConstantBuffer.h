#pragma once

#include "ResourceBindable.h"
#include "GraphicsThrowMacros.h"
#include <d3d11shader.h>

class ReflectedConstantBuffer : public ResourceBindable {
public:
	class CBufferException : public Exception{
	public:
		CBufferException(int line, const char* file) noexcept;
		const char* GetType() const noexcept override;
	};
	struct ConstantBufferVariable {
		D3D11_SHADER_VARIABLE_DESC desc;
		D3D11_SHADER_TYPE_DESC typeDesc;
	};

public:

	ReflectedConstantBuffer(std::vector<ConstantBufferVariable> variables, unsigned int slot);
	/*
	ConstantBuffer(unsigned int slot, bool m_dynamic, const T& buffer) : m_dynamic(m_dynamic), ResourceBindable(slot){
		INFOMAN;

		D3D11_BUFFER_DESC cbd = {};
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		if (m_dynamic) {
			cbd.Usage = D3D11_USAGE_DYNAMIC;
			cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		}
		else {
			cbd.Usage = D3D11_USAGE_IMMUTABLE;
			cbd.CPUAccessFlags = 0;
		}
		cbd.MiscFlags = 0u;
		cbd.ByteWidth = sizeof(T);
		cbd.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA sd = {};
		sd.pSysMem = &buffer;
		sd.SysMemPitch = 0;
		sd.SysMemSlicePitch = 0;

		GFX_THROW_INFO(GetDevice()->CreateBuffer(&cbd, &sd, m_constantBuffer.GetAddressOf()));
	}

	ConstantBuffer(unsigned int slot, T& buffer) : ConstantBuffer(slot) {
		SetBuffer(buffer);
	}
	*/

	~ReflectedConstantBuffer() {
		m_constantBuffer->Release();
	}

	virtual void Bind() const override = 0;
	virtual void Unbind() const override = 0;

	bool SetFloat(const char* name, float value);
	bool SetVector4(const char* name, float* data);
	bool SetMat4(const char* name, float* data);

	void Update();

private:
	bool SetVariable(D3D_SHADER_VARIABLE_CLASS varClass, const char* name, void* data, size_t size);
protected:
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer;
	char* m_buffer;
	std::vector<ConstantBufferVariable> m_variables;
	size_t m_size;
	bool m_dynamic;
	bool m_dirty = true;
};

class ReflectedVertexConstantBuffer : public ReflectedConstantBuffer{
public:
	ReflectedVertexConstantBuffer(std::vector<ConstantBufferVariable> variables, unsigned int slot) : ReflectedConstantBuffer(variables, slot) {}
	void Bind() const override {
		GetContext()->VSSetConstantBuffers(m_slot, 1u, m_constantBuffer.GetAddressOf());
	}
	void Unbind() const override {}
};

class ReflectedPixelConstantBuffer : public ReflectedConstantBuffer {
public:
	ReflectedPixelConstantBuffer(std::vector<ConstantBufferVariable> variables, unsigned int slot) : ReflectedConstantBuffer(variables, slot) {}
	void Bind() const override {
		GetContext()->PSSetConstantBuffers(m_slot, 1u, m_constantBuffer.GetAddressOf());
	}
	void Unbind() const override{}
};
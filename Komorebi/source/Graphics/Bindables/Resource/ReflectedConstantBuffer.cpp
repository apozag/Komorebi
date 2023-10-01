#include "Graphics/Bindables/Resource/ReflectedConstantBuffer.h"
#include "Core/Reflection/ReflectionImplMacros.h"
#include <sstream>

namespace gfx {

	ReflectedConstantBuffer::CBufferException::CBufferException(int line, const char* file) noexcept : Exception(line, file) {
	}
	const char* ReflectedConstantBuffer::CBufferException::GetType() const noexcept {
		return "Constant Buffer Update Exception";
	}

	ReflectedConstantBuffer::ReflectedConstantBuffer(std::vector<ConstantBufferVariable> variables, unsigned int slot) : m_dynamic(true), ResourceBindable(slot) {

		// Allocate total size
		D3D11_SHADER_VARIABLE_DESC desc = variables[variables.size() - 1].desc;
		m_size = desc.StartOffset + desc.Size;
		if (m_size % 16 != 0) m_size += 16 - m_size % 16;
		m_buffer = (float*)malloc(m_size);
		ZeroMemory(m_buffer, m_size);

		m_variables = variables;

		// Create cbuffer object
		INFOMAN

			D3D11_BUFFER_DESC cbd = {};
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0u;
		cbd.ByteWidth = (unsigned int)m_size;
		cbd.StructureByteStride = 0;

		GFX_THROW_INFO(GetDevice()->CreateBuffer(&cbd, NULL, m_constantBuffer.GetAddressOf()));
	}

	bool ReflectedConstantBuffer::HasFloat(const char* name) {
		return GetVariable(D3D_SVC_SCALAR, name) != nullptr;
	}
	bool ReflectedConstantBuffer::HasVector4(const char* name) {
		return GetVariable(D3D_SVC_VECTOR, name) != nullptr;
	}

	float ReflectedConstantBuffer::GetFloat(const char* name) {
		float* ptr = (float*)GetVariable(D3D_SVC_SCALAR, name);
		return ptr != nullptr? *ptr : 0.f;
	}
	void ReflectedConstantBuffer::GetVector4(const char* name, float* values) {
		memcpy(values, GetVariable(D3D_SVC_VECTOR, name), 4*sizeof(float));
	}

	bool ReflectedConstantBuffer::SetFloat(const char* name, float value) {
		return SetVariable(D3D_SVC_SCALAR, name, &value, sizeof(float));
	}
	bool ReflectedConstantBuffer::SetVector4(const char* name, float* data) {
		return SetVariable(D3D_SVC_VECTOR, name, data, 4 * sizeof(float));
	}
	bool ReflectedConstantBuffer::SetMat4(const char* name, float* data) {
		return SetVariable(D3D_SVC_MATRIX_ROWS, name, data, 16 * sizeof(float));
	}

	void ReflectedConstantBuffer::Update() {
		if (!m_dynamic || !m_dirty) return;
		INFOMAN

			D3D11_MAPPED_SUBRESOURCE msr;
		GFX_THROW_INFO(GetContext()->Map(
			m_constantBuffer.Get(), 0u,
			D3D11_MAP_WRITE_DISCARD, 0u,
			&msr
		));
		memcpy(msr.pData, m_buffer, m_size);
		GetContext()->Unmap(m_constantBuffer.Get(), 0u);

		m_dirty = false;
	}

	void* ReflectedConstantBuffer::GetVariable(D3D_SHADER_VARIABLE_CLASS varClass, const char* name) {
		try {
			for (ConstantBufferVariable variable : m_variables) {
				if (variable.typeDesc.Class == varClass && !std::strcmp(variable.desc.Name, name)) {
					size_t offset = variable.desc.StartOffset / sizeof(float);
					return m_buffer + offset;
				}
			}
		}
		catch (Exception e) {
			throw CBufferException(__LINE__, __FILE__);
		}
		return nullptr;
	}

	bool ReflectedConstantBuffer::SetVariable(D3D_SHADER_VARIABLE_CLASS varClass, const char* name, void* data, size_t size) {
		try {
			for (ConstantBufferVariable variable : m_variables) {
				if (variable.typeDesc.Class == varClass && !std::strcmp(variable.desc.Name, name)) {
					size_t offset = variable.desc.StartOffset / sizeof(float);
					memcpy(m_buffer + offset, data, size);
					m_dirty = true;
					return true;
				}
			}
		}
		catch (Exception e) {
			throw CBufferException(__LINE__, __FILE__);
		}
		return false;
	}

	REFLECT_STRUCT_BEGIN(ReflectedVertexConstantBuffer, ResourceBindable)
	REFLECT_STRUCT_END(ReflectedVertexConstantBuffer)

	REFLECT_STRUCT_BEGIN(ReflectedPixelConstantBuffer, ResourceBindable)
	REFLECT_STRUCT_END(ReflectedPixelConstantBuffer)

	REFLECT_STRUCT_BEGIN(ReflectedGeomConstantBuffer, ResourceBindable)
	REFLECT_STRUCT_END(ReflectedGeomConstantBuffer)

}

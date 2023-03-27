#pragma once


#include "Graphics/Bindables/Resource/ResourceBindable.h"
#include "Graphics/Graphics.h"
#include "Graphics/GraphicsThrowMacros.h"

namespace gfx {

	template<typename T>
	class ConstantBuffer : public ResourceBindable {
	public:
		ConstantBuffer(unsigned int slot) : m_dynamic(true), m_slot(slot) {

			INFOMAN

				D3D11_BUFFER_DESC cbd = {};
			cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			cbd.Usage = D3D11_USAGE_DYNAMIC;
			cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			cbd.MiscFlags = 0u;
			cbd.ByteWidth = sizeof(T);
			cbd.StructureByteStride = 0;

			GFX_THROW_INFO(GetDevice()->CreateBuffer(&cbd, NULL, m_constantBuffer.GetAddressOf()));
		}


		ConstantBuffer(unsigned int slot, bool m_dynamic, const T& buffer) : m_dynamic(m_dynamic), m_slot(slot) {
			INFOMAN

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

		~ConstantBuffer() {
			//m_constantBuffer->Release();
		}

		virtual void Bind() const override = 0;
		virtual void Unbind() const override = 0;

		void Update() {
			if (!m_dynamic) return;
			INFOMAN
				D3D11_MAPPED_SUBRESOURCE msr;
			GFX_THROW_INFO(GetContext()->Map(
				m_constantBuffer.Get(), 0u,
				D3D11_MAP_WRITE_DISCARD, 0u,
				&msr
			));
			int size = sizeof(T);
			memcpy(msr.pData, &m_cbuffer, size);
			GetContext()->Unmap(m_constantBuffer.Get(), 0u);
		}

		void SetBuffer(const T& cbuffer) {
			m_cbuffer = cbuffer;
		}

	protected:
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer;
		T m_cbuffer;
		unsigned int m_slot;
		bool m_dynamic;
	};

	template<typename T>
	class VertexConstantBuffer : public ConstantBuffer<T> {
		using ConstantBuffer<T>::m_constantBuffer;
		using ConstantBuffer<T>::m_slot;
		using Bindable::GetContext;
	public:
		VertexConstantBuffer(unsigned int slot) : ConstantBuffer<T>(slot) {}
		VertexConstantBuffer(unsigned int slot, bool m_dynamic, const T& buffer) : ConstantBuffer<T>(slot, m_dynamic, buffer) {}
		void Bind() const override {
			GetContext()->VSSetConstantBuffers(m_slot, 1u, m_constantBuffer.GetAddressOf());
		}
		void Unbind() const override {}
	};

	template<typename T>
	class PixelConstantBuffer : public ConstantBuffer<T> {
		using ConstantBuffer<T>::m_constantBuffer;
		using ConstantBuffer<T>::m_slot;
		using Bindable::GetContext;
	public:
		PixelConstantBuffer(unsigned int slot) : ConstantBuffer<T>(slot) {}
		PixelConstantBuffer(unsigned int slot, bool m_dynamic, const T& buffer) : ConstantBuffer<T>(slot, m_dynamic, buffer) {}
		void Bind() const override {
			GetContext()->PSSetConstantBuffers(m_slot, 1u, m_constantBuffer.GetAddressOf());
		}
		void Unbind() const override {}
	};
}

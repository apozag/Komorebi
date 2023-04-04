#pragma once

#include "Graphics/DxgiInfoManager.h"
#include "Core/GameObject.h"
#include "Core/Reflection/ReflectionMacros.h"

struct ID3D11Device;
struct ID3D11DeviceContext;

namespace gfx {

	class DxgiInfoManager;

	class Bindable : public GameObject {
	public:
		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual ~Bindable() = default;
		REFLECT_BASE()
	protected:
		static ID3D11Device* GetDevice();
		static ID3D11DeviceContext* GetContext();
		static gfx::DxgiInfoManager& GetInfoManager();
	};

}

DECLARE_REFLECTION_POINTER(gfx::Bindable)
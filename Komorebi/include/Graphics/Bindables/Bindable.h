#pragma once

#include "Graphics/Graphics.h"
#include "Core/GameObject.h"
#include "Core/Reflection/ReflectionMacros.h"

class Bindable : public GameObject{
public:
	virtual void Bind() const = 0;
	virtual void Unbind() const = 0;
	virtual ~Bindable() = default;
	REFLECT_BASE()
protected:
	static ID3D11Device* GetDevice();
	static ID3D11DeviceContext* GetContext();
	static DxgiInfoManager& GetInfoManager();
};

DECLARE_REFLECTION_POINTER(Bindable)
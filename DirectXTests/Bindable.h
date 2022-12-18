#pragma once

#include "Graphics.h"
#include "GameObject.h"

class Bindable : public GameObject{
public:
	virtual void Bind() const = 0;
	virtual void Unbind() const = 0;
	virtual ~Bindable() = default;
protected:
	static ID3D11Device* GetDevice();
	static ID3D11DeviceContext* GetContext();
	static DxgiInfoManager& GetInfoManager();
};
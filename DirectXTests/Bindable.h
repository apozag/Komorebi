#pragma once

#include "Graphics.h"

class Bindable {
public:
	virtual void Bind() const = 0;
	virtual void Unbind() const = 0;
	virtual void Update() {};
	virtual ~Bindable() = default;
protected:
	static ID3D11Device* GetDevice();
	static ID3D11DeviceContext* GetContext();
	static DxgiInfoManager& GetInfoManager();
};
#pragma once

#include "Graphics.h"

class Bindable {
public:
	virtual void Bind(Graphics& gfx) const = 0;
	virtual void Unbind(Graphics& gfx) const = 0;
	virtual void Update(Graphics& gfx) {};
	virtual ~Bindable() = default;
protected:
	static ID3D11Device* GetDevice(Graphics& gfx);
	static ID3D11DeviceContext* GetContext(Graphics& gfx);
	static DxgiInfoManager& GetInfoManager(Graphics& gfx);
};
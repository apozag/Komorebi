#include "Bindable.h"

ID3D11Device* Bindable::GetDevice(Graphics& gfx) {
	return gfx.m_device;
}
ID3D11DeviceContext* Bindable::GetContext(Graphics& gfx){
	return gfx.m_context;
}
DxgiInfoManager& Bindable::GetInfoManager(Graphics& gfx)
{
#ifndef NDEBUG
	return gfx.infoManager;
#else
	throw std::logic_error("YouFuckedUp! (tried to access gfx.infoManager in Release config)");
#endif
}
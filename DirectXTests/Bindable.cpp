#include "Bindable.h"
#include "Engine.h"

ID3D11Device* Bindable::GetDevice() {
	return Engine::m_window->gfx->m_device.Get();
}
ID3D11DeviceContext* Bindable::GetContext(){
	return Engine::m_window->gfx->m_context.Get();
}
DxgiInfoManager& Bindable::GetInfoManager()
{
#ifndef NDEBUG
	return Engine::m_window->gfx->infoManager;
#else
	throw std::logic_error("YouFuckedUp! (tried to access gfx.infoManager in Release config)");
#endif
}
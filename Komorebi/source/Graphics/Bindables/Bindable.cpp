#include "Graphics/Bindables/Bindable.h"
#include "Core/Engine.h"
#include "Core/Window.h"

ID3D11Device* Bindable::GetDevice() {
	return Engine::m_window->m_gfx->m_device.Get();
}
ID3D11DeviceContext* Bindable::GetContext(){
	return Engine::m_window->m_gfx->m_context.Get();
}
DxgiInfoManager& Bindable::GetInfoManager()
{
#ifndef NDEBUG
	return Engine::m_window->m_gfx->infoManager;
#else
	throw std::logic_error("YouFuckedUp! (tried to access gfx.infoManager in Release config)");
#endif
}

REFLECT_STRUCT_BASE_VIRTUAL_BEGIN(Bindable)
REFLECT_STRUCT_END(Bindable)

IMPLEMENT_REFLECTION_POINTER(Bindable)
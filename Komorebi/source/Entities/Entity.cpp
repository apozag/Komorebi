#include <d3d11.h>

#include "Entities/Entity.h"

#include "Core/Math/Transform.h"
#include "Core/Engine.h"
#include "Core/Window.h"

Renderer* Entity::GetRenderer() const { return Engine::m_renderer; }
ScriptDispatcher* Entity::GetScriptDispatcher() const { return &Engine::m_scriptDispatcher; }
const Graphics* Entity::GetGraphics() const { return Engine::m_window->m_gfx; }
const Window* Entity::GetWindow() const { return Engine::m_window; }

REFLECT_STRUCT_BASE_VIRTUAL_BEGIN(Entity)
REFLECT_STRUCT_MEMBER(m_tag)
REFLECT_STRUCT_MEMBER(m_node)
REFLECT_STRUCT_END(Entity)

IMPLEMENT_REFLECTION_POINTER(Entity)

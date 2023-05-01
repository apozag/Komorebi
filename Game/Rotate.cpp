#include "Rotate.h"

#include "Core/Reflection/ReflectionImplMacros.h"

#include "Core/Engine.h"
#include "Scene/Node.h"

void Rotate::Run(Node* node) {

}

void Rotate::Update(Node * node) {
	node->m_localTransform.RotateEuler({ 0,0,3.14f * Engine::GetDeltaTime() });
}

REFLECT_STRUCT_BEGIN(Rotate, Script)
REFLECT_STRUCT_END(Rotate)

IMPLEMENT_REFLECTION_POINTER(Rotate)
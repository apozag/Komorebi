#include "Entities/Script.h"

#include "Core/Reflection/ReflectionImplMacros.h"

#include "Core/ScriptDispatcher.h"

void Script::Insert(Node* node, const Transform& worldTransform) {
	GetScriptDispatcher()->SubmitScript(this, node);
}

REFLECT_STRUCT_VIRTUAL_BEGIN(Script, Entity)
REFLECT_STRUCT_END(Script)
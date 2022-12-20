#include "Script.h"
#include "ScriptDispatcher.h"

void Script::Insert(Node* node, const Transform& worldTransform) {
	GetScriptDispatcher()->SubmitScript(this, node);
}

REFLECT_STRUCT_BEGIN(Script, Entity)
REFLECT_STRUCT_END()
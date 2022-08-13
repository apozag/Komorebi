#include "Script.h"
#include "ScriptDispatcher.h"

void Script::Insert(const Transform& worldTransform) {
	GetScriptDispatcher()->SubmitScript(this);
}
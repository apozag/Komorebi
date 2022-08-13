#include "ScriptDispatcher.h"
#include "Script.h"

void  ScriptDispatcher::SubmitScript(Script* script) {
	if (!script->m_initialized) m_startScripts.push_back(script);
	m_updateScripts.push_back(script);
}

void ScriptDispatcher::DispatchScripts() {
	for (Script* script : m_startScripts) {
		script->Start();
		script->m_initialized = true;
	}
	for (Script* script : m_updateScripts) script->Update();
}
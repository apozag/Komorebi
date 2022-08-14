#include "ScriptDispatcher.h"
#include "Script.h"

void  ScriptDispatcher::SubmitScript(Script* script, Node* node) {
	if (!script->m_initialized) m_startJobs.push_back({ script, node });
	m_updateJobs.push_back({ script, node });
}

void ScriptDispatcher::DispatchScripts() {
	for (ScriptJob job : m_startJobs) {
		job.script->Start(job.node);
		job.script->m_initialized = true;
	}
	for (ScriptJob job : m_updateJobs)
	{
		job.script->Update(job.node);
	}

	m_startJobs.clear();
	m_updateJobs.clear();
}
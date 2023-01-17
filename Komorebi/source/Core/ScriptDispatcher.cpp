#include "Core/ScriptDispatcher.h"
#include "Entities/Script.h"

void  ScriptDispatcher::SubmitScript(Script* script, Node* node) {
	if (!script->m_started) m_startJobs.push_back({ script, node });
	m_updateJobs.push_back({ script, node });
}

void ScriptDispatcher::DispatchScripts() {
	for (ScriptJob job : m_startJobs) {
		job.script->Run(job.node);
		job.script->m_started = true;
	}
	for (ScriptJob job : m_updateJobs)
	{
		job.script->Update(job.node);
	}

	m_startJobs.clear();
	m_updateJobs.clear();
}
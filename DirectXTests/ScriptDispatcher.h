#pragma once

#include <vector>

class Script;

class ScriptDispatcher {
public:
	void SubmitScript(Script* script);
	void DispatchScripts();
private:
	std::vector<Script*> m_startScripts;
	std::vector<Script*> m_updateScripts;
};
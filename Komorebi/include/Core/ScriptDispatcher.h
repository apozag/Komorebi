#pragma once

#include <vector>

class Script;
class Node;

class ScriptDispatcher {
private:
	struct ScriptJob {
		Script* script;
		Node* node;
	};
public:
	void SubmitScript(Script* script, Node* node);
	void DispatchScripts();
private:
	std::vector<ScriptJob> m_startJobs;
	std::vector<ScriptJob> m_updateJobs;
};
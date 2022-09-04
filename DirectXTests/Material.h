#pragma once
#include <vector>

class Pass;
class ResourceBindable;

class Material {
public:
	Material();

	void Bind();
	void Unbind();

	void AddBindable(ResourceBindable* bindable);
	void AddPass(Pass* pass);
	const std::vector<Pass*>& GetPasses() { return m_passes; }
	unsigned int GetIdx() { return m_idx; }
private:
	std::vector<ResourceBindable*> m_binds;
	std::vector<Pass*> m_passes;
	unsigned int m_idx;
	static unsigned int static_idx;
};
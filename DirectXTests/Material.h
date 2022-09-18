#pragma once
#include <vector>

class Pass;
class ResourceBindable;
class ReflectedConstantBuffer;
class PixelShader;
class VertexShader;

class Material {
public:
	Material();

	void Bind();
	void Unbind();

	void AddBindable(ResourceBindable* bindable);
	void AddPass(Pass* pass);
	const std::vector<Pass*>& GetPasses() { return m_passes; }
	unsigned int GetIdx() { return m_idx; }

	bool SetFloat(const char* name, float value);
	bool SetVector4(const char* name, float* data);
	bool SetMat4(const char* name, float* data);

private:
	std::vector<ResourceBindable*> m_binds;
	std::vector<ReflectedConstantBuffer*> m_cbuffers;
	std::vector<Pass*> m_passes;
	unsigned int m_idx;
	static unsigned int static_idx;
};
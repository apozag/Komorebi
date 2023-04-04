#pragma once
#include <vector>

#include "Core/GameObject.h"
#include "Core/Reflection/ReflectionMacros.h"
#include "Graphics\Bindables\Resource\ResourceBindable.h"
#include "Graphics\Pass.h"

namespace gfx {

	//class Pass;
	//class ResourceBindable;
	class ReflectedConstantBuffer;
	class PixelShader;
	class VertexShader;

	class Material : public GameObject {
	public:
		Material() : m_idx(static_idx++) {}

		~Material();

		void Setup() override;

		void Bind();
		void Unbind();

		void AddBindable(ResourceBindable* bindable);
		void AddPass(Pass* pass);
		const std::vector<Pass*>& GetPasses() { return m_passes; }
		const std::vector<ResourceBindable*>& GetBinds() { return m_binds; }
		unsigned int GetIdx() { return m_idx; }

		bool SetFloat(const char* name, float value);
		bool SetVector4(const char* name, float* data);
		bool SetMat4(const char* name, float* data);

		REFLECT_BASE()

	private:

		void ProcessPass(Pass* pass);

		std::vector<ReflectedConstantBuffer*> m_cbuffers;
		const unsigned int m_idx;
		static unsigned int static_idx;

		//////////////////
		// Serializable
		//////////////////

		std::vector<Pass*> m_passes;
		std::vector<ResourceBindable*> m_binds;

	};
}

DECLARE_REFLECTION_POINTER(gfx::Material)
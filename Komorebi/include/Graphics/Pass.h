#pragma once

#include <vector>

#include "Core/GameObject.h"
#include "Core/Reflection/ReflectionMacros.h"

namespace gfx {

	class PixelShader;
	class VertexShader;
	class StateBindable;

#define PASSLAYER_OPAQUE		0u
#define PASSLAYER_SKYBOX		1u
#define PASSLAYER_PREPASS		2u
#define PASSLAYER_TRANSPARENT	3u
#define PASSLAYER_SCREEN		4u

	class Pass : public GameObject {
	public:
		Pass() : m_layer(PASSLAYER_OPAQUE), m_idx(static_idx++) {}
		Pass(VertexShader* vs, PixelShader* ps, unsigned int layer, bool skinned = false);
		Pass(const char* vsFilename, const char* psFilename, unsigned int m_layer, bool skinned = false);
		~Pass();

		void Setup() override;

		void AddBindable(StateBindable* bind);
		void Bind();
		void Unbind();
		unsigned char GetIdx() const { return m_idx; }
		PixelShader const* GetPixelShader() const { return m_pixelShader; }
		VertexShader const* GetVertexShader() const { return m_vertexShader; }

		REFLECT_BASE()

	public:
		unsigned int m_layer;
	private:
		std::string m_PSFilename;
		std::string m_VSFilename;
		OWNED_PTR(PixelShader) m_pixelShader;
		OWNED_PTR(VertexShader) m_vertexShader;
		std::vector<OWNED_PTR(StateBindable)> m_binds;
		bool m_skinned;
		static unsigned char static_idx;
		const unsigned char m_idx;
		bool m_enabled = true;
	};
}

DECLARE_REFLECTION_POINTER(gfx::Pass)

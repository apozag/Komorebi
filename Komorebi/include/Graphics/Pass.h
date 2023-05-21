#pragma once

#include <vector>

#include "Core/GameObject.h"
#include "Core/Reflection/ReflectionMacros.h"

namespace gfx {

	class PixelShader;
	class VertexShader;
	class StateBindable;

#define PASSLAYER_FW_OPAQUE		0u
#define PASSLAYER_SKYBOX		1u
#define PASSLAYER_PREPASS		2u
#define PASSLAYER_TRANSPARENT	3u
#define PASSLAYER_SCREEN		4u	

	class ConstantBufferCache : public GameObject{
		public:
		struct VarInfo : public GameObject {
			enum VarType {
				SCALAR,
				VECTOR
			};
			std::string varName;
			VarType varType;
			size_t dataIdx;
			REFLECT_BASE()
		};
		std::vector<VarInfo> varsInfo;
		std::vector<float> varsData;

		void Clear();

		REFLECT_BASE()
	};	

	class Pass : public GameObject {
	public:
		Pass() : m_layer(PASSLAYER_FW_OPAQUE), m_idx(static_idx++) {}
		Pass(VertexShader* vs, PixelShader* ps, unsigned int layer, bool skinned = false);
		Pass(const char* vsFilename, const char* psFilename, unsigned int m_layer, bool skinned = false);
		~Pass();

		void Setup() override;

		void AddBindable(StateBindable* bind);
		void Bind()const;
		void Unbind()const;
		unsigned char GetIdx() const { return m_idx; }
		PixelShader const* GetPixelShader() const { return m_pixelShader; }
		VertexShader const* GetVertexShader() const { return m_vertexShader; }
		bool DoesIgnoreFrustumCulling() const { return m_ignoreFrustumCulling; }

		REFLECT_BASE()

	public:
		unsigned int m_layer;
		ConstantBufferCache m_cbuffCache;

	private:
		std::string m_PSFilename;
		std::string m_VSFilename;
		OWNED_PTR(PixelShader) m_pixelShader;
		OWNED_PTR(VertexShader) m_vertexShader;
		std::vector<OWNED_PTR(StateBindable)> m_binds;		
		bool m_skinned;
		static unsigned char static_idx;
		const unsigned char m_idx;
		bool m_ignoreFrustumCulling = false;
		bool m_enabled = true;
	};
}

DECLARE_REFLECTION_VECTOR(gfx::ConstantBufferCache::VarInfo)

DECLARE_REFLECTION_ENUM(gfx::ConstantBufferCache::VarInfo::VarType)

DECLARE_REFLECTION_POINTER(gfx::Pass)
DECLARE_REFLECTION_VECTOR(gfx::Pass*)
DECLARE_REFLECTION_VECTOR(OWNED_PTR(gfx::Pass))

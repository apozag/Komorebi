#pragma once

#include <vector>

#include "GameObject.h"
#include "ReflectionMacros.h"

class PixelShader;
class VertexShader;
class StateBindable;

#define PASSLAYER_OPAQUE		0u
#define PASSLAYER_SKYBOX		1u
#define PASSLAYER_PREPASS		2u
#define PASSLAYER_TRANSPARENT	3u
#define PASSLAYER_SCREEN		4u

class Pass : public GameObject{
public:
	Pass() : m_layer(PASSLAYER_OPAQUE), m_idx(static_idx++) {}
	Pass(VertexShader* vs, PixelShader* ps, unsigned int layer, bool skinned = false);
	Pass(const char* vsFilename, const char* psFilename, unsigned int m_layer, bool skinned = false);
	~Pass() {};

	void Setup() override;

	void AddBindable(StateBindable* bind);
	void Bind( );
	void Unbind( );
	unsigned char GetIdx() const { return m_idx; }
	const PixelShader const* GetPixelShader() const { return m_pixelShader; }
	const VertexShader const* GetVertexShader() const { return m_vertexShader; }

	REFLECT_BASE()

public:
	unsigned int m_layer;	
private:
	PixelShader* m_pixelShader;
	VertexShader* m_vertexShader;
	std::vector<StateBindable*> m_binds;
	bool m_skinned;
	static unsigned char static_idx;
	const unsigned char m_idx;
};

DECLARE_REFLECTION_POINTER(Pass)
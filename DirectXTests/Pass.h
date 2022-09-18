#pragma once

#include <vector>

class PixelShader;
class VertexShader;
class StateBindable;

#define PASSLAYER_OPAQUE		0u
#define PASSLAYER_SKYBOX		1u
#define PASSLAYER_PREPASS		2u
#define PASSLAYER_TRANSPARENT	3u
#define PASSLAYER_SCREEN		4u

class Pass {
public:
	Pass( const char* vsFilename, const char* psFilename, unsigned int layer, bool skinned = false);
	~Pass() {};
	void AddBindable(StateBindable* bind);
	void Bind( );
	void Unbind( );
	unsigned char GetIdx() const { return m_slot; }
	const PixelShader const* GetPixelShader() const { return m_pixelShader; }
	const VertexShader const* GetVertexShader() const { return m_vertexShader; }
public:
	const unsigned int layer;	
private:
	PixelShader* m_pixelShader;
	VertexShader* m_vertexShader;
	std::vector<StateBindable*> m_binds;
	static unsigned char static_idx;
	unsigned char m_slot;
};
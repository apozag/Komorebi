#pragma once

#include "Graphics.h"
#include "StateBindable.h"
#include "Drawable.h"
#include "VertexShader.h"
#include "PixelShader.h"

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
	unsigned char GetIdx() { return m_slot; }
public:
	const unsigned int layer;	
private:
	std::vector<StateBindable*> m_binds;
	static unsigned char static_idx;
	unsigned char m_slot;
};
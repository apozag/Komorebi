#pragma once

#include "Graphics.h"
#include "Bindable.h"
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
	Pass(Graphics& gfx, const char* vsFilename, const char* psFilename, unsigned int layer, bool skinned = false);
	~Pass() {};
	void AddBindable(Bindable* bind);
	void Update(Graphics& gfx);
	void Bind(Graphics& gfx);
	void Unbind(Graphics& gfx);
	unsigned char GetIdx() { return m_idx; }
public:
	const unsigned int layer;	
private:
	std::vector<Bindable*> m_binds;
	static unsigned char static_idx;
	unsigned char m_idx;
};
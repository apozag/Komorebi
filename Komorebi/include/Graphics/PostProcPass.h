#pragma once


#include "Graphics/Pass.h"
#include "Graphics/Bindables/State/VertexShader.h"
#include "Graphics/Bindables/State/PixelShader.h"

class StateBindable;

class PostProcPass : public Pass {
public:
	PostProcPass(const char* psFilename);
	PostProcPass(PixelShader* ps);
private:
	static VertexShader s_vShader;
};
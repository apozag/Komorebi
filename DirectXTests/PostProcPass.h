#pragma once


#include "Pass.h"
#include "VertexShader.h"
#include "PixelShader.h"

class StateBindable;

class PostProcPass : public Pass {
public:
	PostProcPass(const char* psFilename);
	PostProcPass(PixelShader* ps);
private:
	static VertexShader s_vShader;
};
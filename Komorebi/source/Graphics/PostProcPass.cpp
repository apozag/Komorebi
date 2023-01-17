#include "Graphics/PostProcPass.h"
#include "Graphics/Bindables/State/PixelShader.h"
#include "Graphics/Bindables/State/VertexShader.h"

VertexShader PostProcPass::s_vShader = VertexShader("shaders/screenVertex.cso");

PostProcPass::PostProcPass(const char* psFilename) :
	PostProcPass(new PixelShader(psFilename))
{}

PostProcPass::PostProcPass(PixelShader* ps) :
	Pass(&s_vShader, ps, 0, false) 
{}
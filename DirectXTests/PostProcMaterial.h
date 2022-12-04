#pragma once


#include "Material.h"

class PostProcPass;

class PostProcMaterial : public Material {
public:
	PostProcMaterial() : Material() {}
	void AddPass(PostProcPass* pass);
	void AddPass(Pass* pass) = delete;
};
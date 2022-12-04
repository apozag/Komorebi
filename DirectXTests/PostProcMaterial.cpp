#include "PostProcMaterial.h"

void PostProcMaterial::AddPass(PostProcPass* pass) {
	Material::AddPass((Pass*)pass);
}
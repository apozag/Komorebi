#include "Material.h"
#include "ResourceBindable.h"

unsigned int Material::static_idx = 0;

Material::Material() : m_idx (static_idx++){}

void Material::Bind() {
	for (ResourceBindable* bind : m_binds) {
		bind->Bind();
	}
}

void Material::Unbind() {
	for (ResourceBindable* bind : m_binds) {
		bind->Unbind();
	}
}

void Material::AddBindable(ResourceBindable* bindable) {
	m_binds.push_back(bindable);
}

void Material::AddPass(Pass* pass) {
	m_passes.push_back(pass);
}
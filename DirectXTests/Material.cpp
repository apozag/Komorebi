#include "Material.h"
#include "ReflectedConstantBuffer.h"
#include "ResourceBindable.h"
#include "Pass.h"
#include "PixelShader.h"
#include "VertexShader.h"
#include "BindableSlotsInfo.h"
#include "Sampler.h"

unsigned int Material::static_idx = 0;

Material::Material() : m_idx (static_idx++){
	//AddBindable(new Sampler(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP, 0));
}

void Material::Bind() {
	for (ResourceBindable* bind : m_binds) {	
		bind->Update();
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

	// Vertex Shader
	{
		const VertexShader* vs = pass->GetVertexShader();

		ID3D11ShaderReflection* reflection = vs->GetReflection();

		D3D11_SHADER_DESC desc = {};
		reflection->GetDesc(&desc);

		for (unsigned int i = 0; i < desc.ConstantBuffers; ++i)
		{
			unsigned int register_index = 0;
			ID3D11ShaderReflectionConstantBuffer* buffer = reflection->GetConstantBufferByIndex(i);

			D3D11_SHADER_BUFFER_DESC bdesc;
			buffer->GetDesc(&bdesc);

			for (unsigned int k = 0; k < desc.BoundResources; ++k)
			{
				D3D11_SHADER_INPUT_BIND_DESC ibdesc;
				reflection->GetResourceBindingDesc(k, &ibdesc);

				if (!strcmp(ibdesc.Name, bdesc.Name)) {
					register_index = ibdesc.BindPoint;
					break;
				}
			}

			if (register_index < VCBUFF_FREE_SLOT) continue;

			std::vector<ReflectedConstantBuffer::ConstantBufferVariable> variables;
			for (unsigned int j = 0; j < bdesc.Variables; ++j)
			{
				ID3D11ShaderReflectionVariable* variable = buffer->GetVariableByIndex(j);

				D3D11_SHADER_VARIABLE_DESC vdesc;
				variable->GetDesc(&vdesc);
				ID3D11ShaderReflectionType* type = variable->GetType();
				D3D11_SHADER_TYPE_DESC tdesc;
				type->GetDesc(&tdesc);


				variables.push_back({ vdesc, tdesc });
			}
			ReflectedVertexConstantBuffer* cbuff = new ReflectedVertexConstantBuffer(variables, register_index);
			m_cbuffers.push_back(cbuff);
			m_binds.push_back(cbuff);
		}
	}

	// Pixel Shader
	{
		const PixelShader* ps = pass->GetPixelShader();

		ID3D11ShaderReflection* reflection = ps->GetReflection();

		D3D11_SHADER_DESC desc = {};
		reflection->GetDesc(&desc);

		for (unsigned int i = 0; i < desc.ConstantBuffers; ++i)
		{
			unsigned int register_index = 0;
			ID3D11ShaderReflectionConstantBuffer* buffer = reflection->GetConstantBufferByIndex(i);

			D3D11_SHADER_BUFFER_DESC bdesc;
			buffer->GetDesc(&bdesc);

			for (unsigned int k = 0; k < desc.BoundResources; ++k)
			{
				D3D11_SHADER_INPUT_BIND_DESC ibdesc;
				reflection->GetResourceBindingDesc(k, &ibdesc);

				if (!strcmp(ibdesc.Name, bdesc.Name)) {
					register_index = ibdesc.BindPoint;
					break;
				}
			}

			if (register_index < PCBUFF_FREE_SLOT) continue;

			std::vector<ReflectedConstantBuffer::ConstantBufferVariable> variables;
			for (unsigned int j = 0; j < bdesc.Variables; ++j)
			{
				ID3D11ShaderReflectionVariable* variable = buffer->GetVariableByIndex(j);

				D3D11_SHADER_VARIABLE_DESC vdesc;
				variable->GetDesc(&vdesc);
				ID3D11ShaderReflectionType* type = variable->GetType();
				D3D11_SHADER_TYPE_DESC tdesc;
				type->GetDesc(&tdesc);


				variables.push_back({ vdesc, tdesc });
			}
			ReflectedPixelConstantBuffer* cbuff = new ReflectedPixelConstantBuffer(variables, register_index);
			m_cbuffers.push_back(cbuff);
			m_binds.push_back(cbuff);
		}
	}

	// Finally, store pass
	m_passes.push_back(pass);
}

bool Material::SetFloat(const char* name, float value) {
	for (ReflectedConstantBuffer* cbuff : m_cbuffers) {
		if(cbuff->SetFloat(name, value)) return true;
	}
	return false;
}
bool Material::SetVector4(const char* name, float* data) {
	for (ReflectedConstantBuffer* cbuff : m_cbuffers) {
		if (cbuff->SetVector4(name, data)) return true;
	}
	return false;
}
bool Material::SetMat4(const char* name, float* data) {
	for (ReflectedConstantBuffer* cbuff : m_cbuffers) {
		if (cbuff->SetMat4(name, data)) return true;
	}
	return false;
}
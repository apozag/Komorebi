#include "Graphics/Material.h"
#include "Core/Memory/Factory.h"

#include "Core/Reflection/ReflectionImplMacros.h"

#include "Graphics/Bindables/Resource/ReflectedConstantBuffer.h"
#include "Graphics/Bindables/Resource/ResourceBindable.h"
#include "Graphics/Pass.h"
#include "Graphics/Bindables/State/PixelShader.h"
#include "Graphics/Bindables/State/GeometryShader.h"
#include "Graphics/Bindables/State/VertexShader.h"
#include "Graphics/BindableSlotsInfo.h"
#include "Graphics/Bindables/State/RasterizerState.h"

namespace gfx {

	unsigned int Material::static_idx = 0;

	void Material::Setup() {
		for (Pass* pass : m_passes) {
			ProcessPass(pass);
		}
	}

	Material::~Material() {
	}

	void Material::Bind() const {
		for (ResourceBindable* bind : m_binds) {
			bind->Update();
			bind->Bind();
		}
	}

	void Material::Unbind() const {
		for (ResourceBindable* bind : m_binds) {
			bind->Unbind();
		}
	}

	void Material::AddBindable(ResourceBindable* bindable) {
		m_binds.push_back(bindable);
	}

	void Material::AddPass(Pass* pass) {
		ProcessPass(pass);
		m_passes.push_back(pass);
	}

	void Material::ProcessPass(Pass* pass) {
		// Vertex Shader
		{
			const VertexShader* vs = pass->GetVertexShader();

			ID3D11ShaderReflection* reflection = vs->GetShaderReflection();

			D3D11_SHADER_DESC desc = {};
			reflection->GetDesc(&desc);

			for (unsigned int i = 0; i < desc.ConstantBuffers; ++i) {
				unsigned int register_index = 0;
				ID3D11ShaderReflectionConstantBuffer* buffer = reflection->GetConstantBufferByIndex(i);

				D3D11_SHADER_BUFFER_DESC bdesc;
				buffer->GetDesc(&bdesc);

				for (unsigned int k = 0; k < desc.BoundResources; ++k) {
					D3D11_SHADER_INPUT_BIND_DESC ibdesc;
					reflection->GetResourceBindingDesc(k, &ibdesc);

					if (!strcmp(ibdesc.Name, bdesc.Name)) {
						register_index = ibdesc.BindPoint;
						break;
					}
				}

				if (register_index < VCBUFF_FREE_SLOT) continue;

				std::vector<ReflectedConstantBuffer::ConstantBufferVariable> variables;
				for (unsigned int j = 0; j < bdesc.Variables; ++j) {
					ID3D11ShaderReflectionVariable* variable = buffer->GetVariableByIndex(j);

					D3D11_SHADER_VARIABLE_DESC vdesc;
					variable->GetDesc(&vdesc);
					ID3D11ShaderReflectionType* type = variable->GetType();
					D3D11_SHADER_TYPE_DESC tdesc;
					type->GetDesc(&tdesc);


					variables.push_back({ vdesc, tdesc });
				}
				ReflectedVertexConstantBuffer* cbuff = memory::Factory::Create<ReflectedVertexConstantBuffer>(variables, register_index);
				m_cbuffers.push_back(cbuff);
				m_binds.push_back(cbuff);
			}
		}

		// Geometry Shader
		{
			const GeometryShader* gs = pass->GetBindable<GeometryShader>();

			if (gs != nullptr) {
				ID3D11ShaderReflection* reflection = gs->GetShaderReflection();

				D3D11_SHADER_DESC desc = {};
				reflection->GetDesc(&desc);

				for (unsigned int i = 0; i < desc.ConstantBuffers; ++i) {
					unsigned int register_index = 0;
					ID3D11ShaderReflectionConstantBuffer* buffer = reflection->GetConstantBufferByIndex(i);

					D3D11_SHADER_BUFFER_DESC bdesc;
					buffer->GetDesc(&bdesc);

					for (unsigned int k = 0; k < desc.BoundResources; ++k) {
						D3D11_SHADER_INPUT_BIND_DESC ibdesc;
						reflection->GetResourceBindingDesc(k, &ibdesc);

						if (!strcmp(ibdesc.Name, bdesc.Name)) {
							register_index = ibdesc.BindPoint;
							break;
						}
					}

					if (register_index < GCBUFF_FREE_SLOT) continue;

					std::vector<ReflectedConstantBuffer::ConstantBufferVariable> variables;
					for (unsigned int j = 0; j < bdesc.Variables; ++j) {
						ID3D11ShaderReflectionVariable* variable = buffer->GetVariableByIndex(j);

						D3D11_SHADER_VARIABLE_DESC vdesc;
						variable->GetDesc(&vdesc);
						ID3D11ShaderReflectionType* type = variable->GetType();
						D3D11_SHADER_TYPE_DESC tdesc;
						type->GetDesc(&tdesc);


						variables.push_back({ vdesc, tdesc });
					}
					ReflectedGeomConstantBuffer* cbuff = memory::Factory::Create<ReflectedGeomConstantBuffer>(variables, register_index);
					m_cbuffers.push_back(cbuff);
					m_binds.push_back(cbuff);
				}
			}
		}

		// Pixel Shader
		{
			const PixelShader* ps = pass->GetPixelShader();

			ID3D11ShaderReflection* reflection = ps->GetShaderReflection();

			D3D11_SHADER_DESC desc = {};
			reflection->GetDesc(&desc);

			for (unsigned int i = 0; i < desc.ConstantBuffers; ++i) {
				unsigned int register_index = 0;
				ID3D11ShaderReflectionConstantBuffer* buffer = reflection->GetConstantBufferByIndex(i);

				D3D11_SHADER_BUFFER_DESC bdesc;
				buffer->GetDesc(&bdesc);

				for (unsigned int k = 0; k < desc.BoundResources; ++k) {
					D3D11_SHADER_INPUT_BIND_DESC ibdesc;
					reflection->GetResourceBindingDesc(k, &ibdesc);

					if (!strcmp(ibdesc.Name, bdesc.Name)) {
						register_index = ibdesc.BindPoint;
						break;
					}
				}

				if (register_index < PCBUFF_FREE_SLOT) continue;

				std::vector<ReflectedConstantBuffer::ConstantBufferVariable> variables;
				for (unsigned int j = 0; j < bdesc.Variables; ++j) {
					ID3D11ShaderReflectionVariable* variable = buffer->GetVariableByIndex(j);

					D3D11_SHADER_VARIABLE_DESC vdesc;
					variable->GetDesc(&vdesc);
					ID3D11ShaderReflectionType* type = variable->GetType();
					D3D11_SHADER_TYPE_DESC tdesc;
					type->GetDesc(&tdesc);


					variables.push_back({ vdesc, tdesc });
				}
				ReflectedPixelConstantBuffer* cbuff = memory::Factory::Create<ReflectedPixelConstantBuffer>(variables, register_index);
				m_cbuffers.push_back(cbuff);
				m_binds.push_back(cbuff);
			}
		}
		
		CopyFromConstantBufferCache();
	}

	void Material::UpdateConstantBufferCache() {
		for (Pass* pass : m_passes) {
			pass->m_cbuffCache.Clear();
			for (ReflectedConstantBuffer* cbuffer : m_cbuffers) {
				bool skipBuffer = false;
				for (const ReflectedConstantBuffer::ConstantBufferVariable& variable : cbuffer->GetVariables()) {					
					ConstantBufferCache::VarInfo varInfo;
					varInfo.varName = variable.desc.Name;
					varInfo.dataIdx = pass->m_cbuffCache.varsData.size();
					switch (variable.typeDesc.Class) {
					case D3D_SVC_SCALAR:
					{
						if (!cbuffer->HasFloat(varInfo.varName.c_str())) {
							skipBuffer = true;
							break;
						}
						varInfo.varType = ConstantBufferCache::VarInfo::VarType::SCALAR;
						pass->m_cbuffCache.varsData.push_back(cbuffer->GetFloat(varInfo.varName.c_str()));
					}
						break;
					case D3D_SVC_VECTOR:
					{
						if (!cbuffer->HasVector(varInfo.varName.c_str())) {
							skipBuffer = true;
							break;
						}

						if (variable.typeDesc.Columns == 2) {
							varInfo.varType = ConstantBufferCache::VarInfo::VarType::VECTOR2;
							float values[2];
							cbuffer->GetVector2(varInfo.varName.c_str(), &(values[0]));
							pass->m_cbuffCache.varsData.push_back(values[0]);
							pass->m_cbuffCache.varsData.push_back(values[1]);
						}
						else {
							varInfo.varType = ConstantBufferCache::VarInfo::VarType::VECTOR4;							
							float values[4];
							cbuffer->GetVector4(varInfo.varName.c_str(), &(values[0]));
							pass->m_cbuffCache.varsData.push_back(values[0]);
							pass->m_cbuffCache.varsData.push_back(values[1]);
							pass->m_cbuffCache.varsData.push_back(values[2]);
							pass->m_cbuffCache.varsData.push_back(values[3]);
						}												
					}
						break;
					}				
					if (skipBuffer) {
						break;
					}
					pass->m_cbuffCache.varsInfo.push_back(varInfo);
				}
				if (skipBuffer) {
					break;
				}
			}
		}		
	}

	void Material::CopyFromConstantBufferCache() {
		using CacheVarType = ConstantBufferCache::VarInfo::VarType;
		for (Pass* pass : m_passes) {
			for (const ConstantBufferCache::VarInfo& varInfo : pass->m_cbuffCache.varsInfo) {
				float* pValue = &(pass->m_cbuffCache.varsData[varInfo.dataIdx]);
				switch (varInfo.varType) {
				case CacheVarType::SCALAR:
					SetFloat(varInfo.varName.c_str(), *pValue);
					break;
				case CacheVarType::VECTOR2:
					SetVector2(varInfo.varName.c_str(), pValue);
					break;
				case CacheVarType::VECTOR4:
					SetVector4(varInfo.varName.c_str(), pValue);
					break;
				}
			}
		}
	}

	bool Material::SetFloat(const char* name, float value) {
		for (ReflectedConstantBuffer* cbuff : m_cbuffers) {
			if (cbuff->SetFloat(name, value)) return true;
		}
		return false;
	}
	bool Material::SetVector4(const char* name, float* data) {
		for (ReflectedConstantBuffer* cbuff : m_cbuffers) {
			if (cbuff->SetVector4(name, data)) return true;
		}
		return false;
	}
	bool Material::SetVector2(const char* name, float* data) {
		for (ReflectedConstantBuffer* cbuff : m_cbuffers) {
			if (cbuff->SetVector2(name, data)) return true;
		}
		return false;
	}
	bool Material::SetMat4(const char* name, float* data) {
		for (ReflectedConstantBuffer* cbuff : m_cbuffers) {
			if (cbuff->SetMat4(name, data)) return true;
		}
		return false;
	}

	REFLECT_STRUCT_BASE_BEGIN(Material)
	REFLECT_STRUCT_MEMBER(m_passes)
	REFLECT_STRUCT_MEMBER(m_binds)
	REFLECT_STRUCT_END(Material)

}

IMPLEMENT_REFLECTION_POINTER_NAMESPACE(gfx, Material)
IMPLEMENT_REFLECTION_ASSET_POINTER_NAMESPACE(gfx, Material)

typedef const gfx::Material ConstMatPtr;
IMPLEMENT_REFLECTION_POINTER(ConstMatPtr)
IMPLEMENT_REFLECTION_ASSET_POINTER(ConstMatPtr)

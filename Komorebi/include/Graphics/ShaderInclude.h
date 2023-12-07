#pragma once

#include <d3dcompiler.h>
#include <string>

class ShaderInclude : public ID3DInclude {
public:
  // shaderDir: Directory of current shader file, used by #include "FILE"
  // systemDir: Default shader includes directory, used by #include <FILE>
  ShaderInclude(const char* shaderDir, const char* systemDir) :
    m_ShaderDir(shaderDir),
    m_SystemDir(systemDir) {
  }

  HRESULT __stdcall Open(
    D3D_INCLUDE_TYPE IncludeType,
    LPCSTR pFileName,
    LPCVOID pParentData,
    LPCVOID* ppData,
    UINT* pBytes);

  HRESULT __stdcall Close(LPCVOID pData);

private:
  std::string m_ShaderDir;
  std::string m_SystemDir;
};

#include "Graphics\ShaderInclude.h"
#include <fstream>

HRESULT __stdcall ShaderInclude::Open(
  D3D_INCLUDE_TYPE IncludeType,
  LPCSTR pFileName,
  LPCVOID pParentData,
  LPCVOID* ppData,
  UINT* pBytes) {
  try {
    /*
    If pFileName is absolute: finalPath = pFileName.
    If pFileName is relative: finalPath = dir + "\\" + pFileName
    */
    //switch (IncludeType) {
    //case D3D_INCLUDE_LOCAL: // #include "FILE"
    //  common::RelativeToAbsolutePath(&finalPath, m_ShaderDir, pFileName);
    //  break;
    //case D3D_INCLUDE_SYSTEM: // #include <FILE>
    //  common::RelativeToAbsolutePath(&finalPath, m_SystemDir, pFileName);
    //  break;
    //default:
    //  assert(0);
    //}

    /*std::string("($(SolutionDir))/") +*/
    std::string finalPath = std::string("D:/Documents/Komorebi/") + m_SystemDir + "/" + pFileName;

    std::ifstream fileStream;
    fileStream.open(finalPath, std::ios_base::ate);

    size_t fsize = fileStream.tellg();
    fileStream.seekg(0, std::ios::beg);

    fileStream.clear();

    if (fsize) {
      char* data = new char[fsize];
      //fileStream.read(data, fsize);

      char c;
      unsigned charCount = 0u;
      while (fileStream.get(c)) {
        data[charCount++] = c;
      }
      data[charCount] = '\0';

      *ppData = data;
      *pBytes = (UINT)fsize;
    }
    else {
      *ppData = nullptr;
      *pBytes = 0;
    }
    fileStream.close();
    return S_OK;
  }
  catch (const std::exception& err) {
    return E_FAIL;
  }
}

HRESULT __stdcall ShaderInclude::Close(LPCVOID pData) {
  // Here we must correctly free buffer created in Open.
  char* buf = (char*)pData;
  delete[] buf;
  return S_OK;
}
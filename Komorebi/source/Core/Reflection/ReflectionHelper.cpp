#include "Core/Reflection/TypeDescriptors.h"
#include "Core/Reflection/ReflectionHelper.h"

namespace reflection {

  typedef std::unordered_map<std::string, TypeDescriptor*> TypeDict;
  typedef std::vector< ReflectionPtrInfo> PtrInfoVector;
  typedef std::vector<std::string*> StrVector;
  typedef std::vector<PtrIdPair> PtrIdVector;

  struct PtrIdPair {
    void** pPtr;
    intptr_t id;
  };

  struct ReflectionPtrInfo {
    void* ptr;
    intptr_t id;
    size_t size;
  };

  void ReflectionHelper::RegisterTypeDesc(TypeDescriptor* typeDesc) {
    GetTypeDict().insert({ typeDesc->name, typeDesc });
  }

  const TypeDescriptor* ReflectionHelper::GetTypeDesc(const std::string& name) {
    return GetTypeDict()[name];
  }

  void ReflectionHelper::TrackString(std::string* string) {
    GetStrVector().push_back(string);
  }

  void ReflectionHelper::ClearTrackedStrings() {
    for (std::string* strPtr : GetStrVector()) {
      delete strPtr;
    }
    GetStrVector().clear();
  }

  void ReflectionHelper::RegisterPtrId(void* ptr, intptr_t id, size_t size) {
    GetPtrInfoVector().push_back({ptr, id, size});
  }

  void ReflectionHelper::RegisterPendingPtr(void** ptr, intptr_t id) {
    GetPendingPtrVector().push_back({ptr, id});
  }

  void ReflectionHelper::ResolvePendingPointers() {
    for (PtrIdPair& pendingPtr : GetPendingPtrVector()) {
      int i = 0;
      for (; i < GetPtrInfoVector().size(); i++) {
        ReflectionPtrInfo& ptrInfo = GetPtrInfoVector()[i];
        int offset = pendingPtr.id - ptrInfo.id;
        if (offset >= 0 && offset < ptrInfo.size) {
          *pendingPtr.pPtr = (char*)ptrInfo.ptr + offset;
          break;
        }
      }
      if (i >= GetPtrInfoVector().size()) {
        *pendingPtr.pPtr = nullptr;
      }
    }
  }

  std::vector<const TypeDescriptor_Struct*> ReflectionHelper::GetChildTypes(const TypeDescriptor_Struct* typeDesc) {
    std::vector<const TypeDescriptor_Struct*> childTypes;
    TypeDict& typeDict = GetTypeDict();
    for (auto& it : typeDict) {
      const TypeDescriptor_Struct* typeDescStruct = dynamic_cast<const reflection::TypeDescriptor_Struct*>(it.second);
      while(typeDescStruct) {                       
        if (typeDescStruct == typeDesc) {
          childTypes.push_back(static_cast<const reflection::TypeDescriptor_Struct*>(it.second));
          break;
        }
        typeDescStruct = typeDescStruct->parentTypeDesc;
      }
    }
    return childTypes;
  }

  void ReflectionHelper::ClearAll() {
    ClearTrackedStrings();
    GetPtrInfoVector().clear();
    GetPendingPtrVector().clear();
  }

  TypeDict& ReflectionHelper::GetTypeDict() {
    static TypeDict typeDict;
    return typeDict;
  }

  PtrInfoVector& ReflectionHelper::GetPtrInfoVector() {
    static PtrInfoVector ptrInfoVector;
    return ptrInfoVector;
  }

  StrVector& ReflectionHelper::GetStrVector() {
    static StrVector strVector;
    return strVector;
  }

  PtrIdVector& ReflectionHelper::GetPendingPtrVector() {
    static PtrIdVector ptrVector;
    return ptrVector;
  }

}
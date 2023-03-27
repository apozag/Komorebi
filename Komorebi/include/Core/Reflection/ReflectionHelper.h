#pragma once

#include <unordered_map>
#include <vector>
  
namespace reflection {

  struct TypeDescriptor;

  struct PtrIdPair;

  struct ReflectionPtrInfo;

  class ReflectionHelper {
  public:

    static void RegisterTypeDesc(TypeDescriptor* typeDesc);

    static const TypeDescriptor* GetTypeDesc(std::string name);

    static void TrackString(std::string* string);

    static void ClearTrackedStrings();

    static void RegisterPtrId(void* ptr, intptr_t id, size_t size);

    static void RegisterPendingPtr(void** ptr, intptr_t id);

    static void ResolvePendingPointers();

    static std::vector<const TypeDescriptor_Struct*> GetChildTypes(const TypeDescriptor_Struct* typeDesc);

    static void ClearAll();

  private:
    ReflectionHelper();
    
    typedef std::unordered_map<std::string, TypeDescriptor*> TypeDict;
    typedef std::vector< ReflectionPtrInfo> PtrInfoVector;
    typedef std::vector<std::string*> StrVector;
    typedef std::vector<PtrIdPair> PtrIdVector;

    static TypeDict& GetTypeDict();
    static StrVector& GetStrVector();
    static PtrInfoVector& GetPtrInfoVector();
    static PtrIdVector& GetPendingPtrVector();
  };
}

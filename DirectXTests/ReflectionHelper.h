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

    static void RegisterPtrId(void* ptr, unsigned int id, size_t size);

    static void RegisterPendingPtr(void** ptr, unsigned int id);

    static void ResolvePendingPointers();

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

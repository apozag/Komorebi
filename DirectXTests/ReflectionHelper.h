#pragma once

#include <unordered_map>
#include <vector>
  
namespace reflection {

  struct TypeDescriptor;

  class ReflectionHelper {
  public:
    static void RegisterTypeDesc(TypeDescriptor* typeDesc);

    static const TypeDescriptor* GetTypeDesc(std::string name);

    static void TrackString(std::string* string);

    static void ClearTrackedStrings();

  private:
    ReflectionHelper();

    typedef std::unordered_map<std::string, TypeDescriptor*> TypeDict;
    typedef std::vector<std::string*> StrVector;

    static TypeDict& GetTypeDict();

    static StrVector& GetStrVector();
  };
}
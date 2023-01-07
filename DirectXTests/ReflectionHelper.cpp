#include "TypeDescriptors.h"
#include "ReflectionHelper.h"

namespace reflection {
  void ReflectionHelper::RegisterTypeDesc(TypeDescriptor* typeDesc) {
    GetTypeDict().insert({ typeDesc->name, typeDesc });
  }

  const TypeDescriptor* ReflectionHelper::GetTypeDesc(std::string name) {
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

  typedef std::unordered_map<std::string, TypeDescriptor*> TypeDict;
  typedef std::vector<std::string*> StrVector;

  TypeDict& ReflectionHelper::GetTypeDict() {
    static TypeDict typeDict;
    return typeDict;
  };

  StrVector& ReflectionHelper::GetStrVector() {
    static StrVector strVector;
    return strVector;
  };
}
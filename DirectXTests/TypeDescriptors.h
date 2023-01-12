#pragma once

#include <string>
#include <vector>

#include "rapidxml/rapidxml_ext.hpp"
#include "rapidxml/rapidxml.hpp"

#include "ReflectionHelper.h"

namespace rapidxml {
  template<class Ch = char>
  class xml_node;
  template<class Ch = char>
  class xml_document;
}

namespace reflection {

  struct TypeDescriptor;

  // Declare the function template that handles primitive types such as int, std::string, etc.:
  template <typename T>
  TypeDescriptor* getPrimitiveDescriptor();

  // A helper class to find TypeDescriptors in different ways:
  struct DefaultResolver {
    template <typename T> static char func(decltype(&T::GetReflection));
    template <typename T> static int func(...);
    template <typename T>
    struct IsReflected {
      enum { value = (sizeof(func<T>(nullptr)) == sizeof(char)) };
    };

    // This version is called if T has a static method named "GetReflection":
    template <typename T, typename std::enable_if<IsReflected<T>::value, int>::type = 0>
    static const TypeDescriptor* get() {
      return &T::GetReflection();
    }

    // This version is called otherwise:
    template <typename T, typename std::enable_if<!IsReflected<T>::value, int>::type = 0>
    static const TypeDescriptor* get() {
      return getPrimitiveDescriptor<T>();
    }
  };

  // This is the primary class template for finding all TypeDescriptors:
  template <typename T>
  struct TypeResolver {
    static const TypeDescriptor* get() {
      return DefaultResolver::get<T>();
    }
  };

  //--------------------------------------------------------
  // Base class of all type descriptors
  //--------------------------------------------------------

  struct TypeDescriptor {
    const char* name;
    size_t size;

    void (*construct)(void*);

    TypeDescriptor(const char* name, size_t size) : name{ name }, size{ size }, construct([](void* obj) {})
    {}
    virtual ~TypeDescriptor() {}
    virtual const std::string getFullName() const;
    virtual void deserialize(void* obj, const rapidxml::xml_node<>* xmlNode) const {};
    virtual void serialize(const void* obj, const char* varName, rapidxml::xml_node<>* xmlParent, rapidxml::xml_document<>* doc) const {};
  protected:
    void serializeMembersVisit(const TypeDescriptor* typeDesc, const void* obj, rapidxml::xml_node<>* xmlNode, rapidxml::xml_document<>* doc) const {
      typeDesc->serializeMembers(obj, xmlNode, doc);
    }
  private:
    virtual void serializeMembers(const void* obj, rapidxml::xml_node<>* xmlNode, rapidxml::xml_document<>* doc) const {};
  };

  //--------------------------------------------------------
  // Type descriptor for user-defined structs/classes
  //--------------------------------------------------------

  struct TypeDescriptor_Struct : public TypeDescriptor {
    struct Member {
      const char* name;
      size_t offset;
      const TypeDescriptor* type;
    };

    std::vector<Member> members;

    const TypeDescriptor* parentTypeDesc;

    void (*setup)(void*);

    TypeDescriptor_Struct(void (*init)(TypeDescriptor_Struct*)) : TypeDescriptor{ nullptr, 0 }
    {
      init(this);
    }
    TypeDescriptor_Struct(const char* name, size_t size, const std::initializer_list<Member>& init) : TypeDescriptor{ nullptr, 0 }, members{ init }
    {}

    virtual void deserialize(void* obj, const rapidxml::xml_node<>* xmlNode) const override;

    virtual void serialize(const void* obj, const char* varName, rapidxml::xml_node<>* xmlParent, rapidxml::xml_document<>* doc) const override;

    int getFirstMemberIdx() const;

  protected:
    virtual void serializeMembers(const void* obj, rapidxml::xml_node<>* xmlNode, rapidxml::xml_document<>* doc) const override;
  };

  //--------------------------------------------------------
  // Type descriptor for pointer to struct/class
  //--------------------------------------------------------

  template <typename T>
  struct TypeDescriptor_Ptr : TypeDescriptor {

    TypeDescriptor_Ptr(void (*init)(TypeDescriptor_Ptr*)) : TypeDescriptor{ nullptr, 0 }
    {
      init(this);
    }
    TypeDescriptor_Ptr(const char* name, size_t size) : TypeDescriptor{ nullptr, 0 }
    {}

    template <typename T> static char func(decltype(&T::GetReflectionDerived));
    template <typename T> static int func(...);
    template <typename T>
    struct IsReflected {
      enum { value = (sizeof(func<T>(nullptr)) == sizeof(char)) };
    };

    // This version is called if T has a static method named "GetReflectionDerived":
    template <typename T, typename std::enable_if<IsReflected<T>::value, int>::type = 0>
    static const TypeDescriptor* getReferenced(const void* obj) {
      return &(*(T**)obj)->GetReflectionDerived();
    }

    // This version is called otherwise:
    template <typename T, typename std::enable_if<!IsReflected<T>::value, int>::type = 0>
    static const TypeDescriptor* getReferenced(const void* /* unused */) {
      return getPrimitiveDescriptor<T>();
    }

    virtual void deserialize(void* obj, const rapidxml::xml_node<>* xmlNode) const override {
      void** ppObj = (void**)obj;
      char* name = xmlNode->name();
      name[xmlNode->name_size()-1] = '\0';
      const TypeDescriptor* typeDesc = ReflectionHelper::GetTypeDesc(name);
      *ppObj = malloc(typeDesc->size);
      typeDesc->deserialize(*ppObj, xmlNode);
    };

    virtual void serialize(const void* obj, const char* varName, rapidxml::xml_node<>* xmlParent, rapidxml::xml_document<>* doc) const override {
      const TypeDescriptor* typeDesc = getReferenced<T>(obj);
      if (!varName) {
        std::string* name = new std::string(typeDesc->getFullName() + "*");
        ReflectionHelper::TrackString(name);
        varName = name->c_str();
      }
      const void** ppObj = (const void**)obj;
      typeDesc->serialize(*ppObj, varName, xmlParent, doc);
    };
  };

  //--------------------------------------------------------
  // Type descriptors for std::vector
  //--------------------------------------------------------

  struct TypeDescriptor_StdVector : TypeDescriptor {
    const TypeDescriptor* itemType;
    size_t(*getSize)(const void*);
    void* (*getItem)(const void*, size_t);
    void (*resize) (void*, size_t);

    template <typename ItemType>
    TypeDescriptor_StdVector(ItemType*) :
      TypeDescriptor {
      "std::vector<>", sizeof(std::vector<ItemType>)
    },
      itemType{ TypeResolver<ItemType>::get() }
      {
        getSize = [](const void* vecPtr) -> size_t {
          const auto& vec = *(const std::vector<ItemType>*) vecPtr;
          return vec.size();
        };
        getItem = [](const void* vecPtr, size_t index) -> void* {
          auto& vec = *(std::vector<ItemType>*) vecPtr;
          return &(vec[index]);
        };
        resize = [](void* vecPtr, size_t size) {
          auto vec = (std::vector<ItemType>*) vecPtr;
          vec->resize(size);
        };
        construct = [](void* obj) {
          new(obj) std::vector<ItemType>();
        };
      }

    virtual const std::string getFullName() const override;

    virtual void deserialize(void* obj, const rapidxml::xml_node<>* xmlNode) const override;

    virtual void serialize(const void* obj, const char* varName, rapidxml::xml_node<>* xmlParent, rapidxml::xml_document<>* doc) const override;
  };

  // Partially specialize TypeResolver<> for std::vectors:
  template <typename T>
  class TypeResolver<std::vector<T>> {
  public:
    static TypeDescriptor* get() {
      static TypeDescriptor_StdVector typeDesc{ (T*) nullptr };
      return &typeDesc;
    }
  };

} // namespace reflect

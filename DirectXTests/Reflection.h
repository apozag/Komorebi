#pragma once

#include "rapidxml/rapidxml_ext.hpp"
#include "rapidxml/rapidxml.hpp"
#include "SceneLoader.h"

#include <vector>
#include <iostream>
#include <cstring>
#include <string>
#include <cstddef>

namespace reflect {

  //--------------------------------------------------------
  // Base class of all type descriptors
  //--------------------------------------------------------

  struct TypeDescriptor {
    const char* name;
    size_t size;

    TypeDescriptor(const char* name, size_t size) : name{ name }, size{ size } 
    {}
    virtual ~TypeDescriptor() {}
    virtual std::string getFullName() const { return name; }
    virtual void dump(const void* obj, std::ostream& os, int indentLevel = 0) const = 0;
    virtual void deserialize(void* obj, const rapidxml::xml_node<>* xmlNode) const {};
    virtual void serialize(const void* obj, rapidxml::xml_node<>* xmlNode, rapidxml::xml_document<>* doc) const {};
  };

  //--------------------------------------------------------
  // Finding type descriptors
  //--------------------------------------------------------

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
  // Type descriptor for user-defined structs/classes
  //--------------------------------------------------------

  struct TypeDescriptor_Struct : TypeDescriptor {
    struct Member {
      const char* name;
      size_t offset;
      const TypeDescriptor* type;
    };

    std::vector<Member> members;

    const TypeDescriptor* parentTypeDesc;

    TypeDescriptor_Struct(void (*init)(TypeDescriptor_Struct*)) : TypeDescriptor{ nullptr, 0 }
    {
      init(this);
    }
    TypeDescriptor_Struct(const char* name, size_t size, const std::initializer_list<Member>& init) : TypeDescriptor{ nullptr, 0 }, members{ init }
    {}
    virtual void dump(const void* obj, std::ostream& os, int indentLevel) const override {
      if(parentTypeDesc) parentTypeDesc->dump(obj, os, indentLevel);
      os << name << " {" << std::endl;
      for (const Member& member : members) {
        os << std::string(4 * (indentLevel + 1), ' ') << member.name << " = ";
        member.type->dump((char*)obj + member.offset, os, indentLevel + 1);
        os << std::endl;
      }
      os << std::string(4 * indentLevel, ' ') << "}";
    }

    virtual void deserialize(void* obj, const rapidxml::xml_node<>* xmlNode) const override {
      if (parentTypeDesc) parentTypeDesc->deserialize(obj, xmlNode);
      typedef rapidxml::xml_node<>* nodePtr;
      nodePtr child = xmlNode->first_node();
      while (child) {
        for (Member member : members) {
          if (!std::strcmp(child->name(), member.name)) {
            member.type->deserialize((char*)obj + member.offset, child);
            break;
          }
        }
        child = child->next_sibling();
      }
    };

    virtual void serialize(const void* obj, rapidxml::xml_node<>* xmlNode, rapidxml::xml_document<>* doc) const override {
      if (parentTypeDesc) parentTypeDesc->serialize(obj, xmlNode, doc);
      typedef rapidxml::xml_node<>* nodePtr;
      for (Member member : members) {
        nodePtr newNode = doc->allocate_node(rapidxml::node_type::node_element, member.name);
        xmlNode->append_node(newNode);
        member.type->serialize((char*)obj + member.offset, newNode, doc);
      }
    };
  };

  //--------------------------------------------------------
  // Type descriptor for pointer to struct/class
  //--------------------------------------------------------

  struct TypeDescriptor_Ptr : TypeDescriptor {

    const reflect::TypeDescriptor_Struct& (*refTypeGetReflection)();

    TypeDescriptor_Ptr(void (*init)(TypeDescriptor_Ptr*)) : TypeDescriptor{ nullptr, 0 }
    {
      init(this);
    }
    TypeDescriptor_Ptr(const char* name, size_t size) : TypeDescriptor{ nullptr, 0 }
    {}
    virtual void dump(const void* obj, std::ostream& os, int indentLevel) const override {
      os << name << " {" << std::endl;
      const void** ppObj = (const void**)obj;
      refTypeGetReflection().dump(&ppObj, os, indentLevel);
      os << std::string(4 * indentLevel, ' ') << "}";
    }

    virtual void deserialize(void* obj, const rapidxml::xml_node<>* xmlNode) const override {
      void** ppObj = (void**)obj;
      refTypeGetReflection().deserialize(*ppObj, xmlNode);
    };

    virtual void serialize(const void* obj, rapidxml::xml_node<>* xmlNode, rapidxml::xml_document<>* doc) const override {
      const void** ppObj = (const void**)obj;
      refTypeGetReflection().serialize(*ppObj, xmlNode, doc);
    };
  };

  //--------------------------------------------------------
  // Type descriptors for std::vector
  //--------------------------------------------------------

  struct TypeDescriptor_StdVector : TypeDescriptor {
    const TypeDescriptor* itemType;
    size_t(*getSize)(const void*);
    const void* (*getItem)(const void*, size_t);
    void (*resize) (void*, size_t);

    template <typename ItemType>
    TypeDescriptor_StdVector(ItemType*) :
      TypeDescriptor{ "std::vector<>", sizeof(std::vector<ItemType>) },
      itemType{ TypeResolver<ItemType>::get() } 
    {
      getSize = [](const void* vecPtr) -> size_t {
        const auto& vec = *(const std::vector<ItemType>*) vecPtr;
        return vec.size();
      };
      getItem = [](const void* vecPtr, size_t index) -> const void* {
        const auto& vec = *(const std::vector<ItemType>*) vecPtr;
        return &(vec[index]);
      };
      resize = [](void* vecPtr, size_t size){
        auto vec = (std::vector<ItemType>*) vecPtr;
        vec->resize(size);
      };
    }
    virtual std::string getFullName() const override {
      return std::string("std::vector<") + itemType->getFullName() + ">";
    }
    virtual void dump(const void* obj, std::ostream& os, int indentLevel) const override {
      size_t numItems = getSize(obj);
      os << getFullName();
      if (numItems == 0) {
        os << "{}";
      }
      else {
        os << "{" << std::endl;
        for (size_t index = 0; index < numItems; index++) {
          os << std::string(4 * (indentLevel + 1), ' ');
          itemType->dump(getItem(obj, index), os, indentLevel + 1);
          os << std::endl;
        }
        os << std::string(4 * indentLevel, ' ') << "}";
      }
    }

    virtual void deserialize(void* obj, const rapidxml::xml_node<>* xmlNode) const override {
      typedef rapidxml::xml_node<>* nodePtr;
      nodePtr child = xmlNode->first_node();
      size_t offset = 0;
      resize(obj, getSize(obj));
      while (child) {
        itemType->deserialize((char*)obj + offset, child);
        offset += itemType->size;
        child = child->next_sibling();
      }
    };

    virtual void serialize(const void* obj, rapidxml::xml_node<>* xmlNode, rapidxml::xml_document<>* doc) const override {
      typedef rapidxml::xml_node<>* nodePtr;
      size_t size = getSize(obj);
      for (int i = 0; i < size; i++) {
        nodePtr newNode = doc->allocate_node(rapidxml::node_type::node_element, itemType->name);
        xmlNode->append_node(newNode);
        itemType->serialize(getItem(obj, i), newNode, doc);
      }
    };
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

#define CAT(a, b) a ## b

#define REFLECT_BASE()  \
  __REFLECT(const)

#define REFLECT()  \
  __REFLECT(const override)

#define __REFLECT(X)	\
  friend struct reflect::DefaultResolver;	\
  static const reflect::TypeDescriptor_Struct& GetReflection() { \
    static reflect::TypeDescriptor_Struct Reflection{initReflection}; \
    return Reflection;  \
  } \
  virtual const reflect::TypeDescriptor_Struct& GetReflectionDerived() X{  \
      return GetReflection();  \
  } \
  static void initReflection(reflect::TypeDescriptor_Struct*);	

#define REFLECT_STRUCT_BASE_BEGIN(type) \
  void type::initReflection(reflect::TypeDescriptor_Struct* typeDesc) { \
    using T = type; \
    typeDesc->name = #type; \
    typeDesc->size = sizeof(T); \
    typeDesc->members = {

#define REFLECT_STRUCT_BEGIN(type, parentType) \
  void type::initReflection(reflect::TypeDescriptor_Struct* typeDesc) { \
    using T = type; \
    typeDesc->name = #type; \
    typeDesc->size = sizeof(T); \
    typeDesc->parentTypeDesc = &parentType::GetReflection(); \
    typeDesc->members = {

#define REFLECT_STRUCT_MEMBER(name) \
      {#name, offsetof(T, name), reflect::TypeResolver<decltype(T::name)>::get()},

#define REFLECT_STRUCT_END() \
    }; \
    SceneLoader::RegisterTypeDesc(typeDesc);  \
  }

#define DECLARE_REFLECTION_PRIMITIVE(type) \
  template <> reflect::TypeDescriptor* reflect::getPrimitiveDescriptor<type>();

#define __IMPLEMENT_REFLECTION_PRIMITIVE_BEGIN(type, tag) \
  struct TypeDescriptor_ ## tag : reflect::TypeDescriptor {  \
    TypeDescriptor_ ## tag() : reflect::TypeDescriptor{ #type, sizeof(type) } {}

#define __IMPLEMENT_REFLECTION_PRIMITIVE_END(type, tag) \
  };  \
  template <> \
  reflect::TypeDescriptor* reflect::getPrimitiveDescriptor<type>() {    \
    static  TypeDescriptor_ ## tag typeDesc;   \
    return &typeDesc;   \
  }

#define IMPLEMENT_REFLECTION_PRIMITIVE(type, tag)	\
  __IMPLEMENT_REFLECTION_PRIMITIVE_BEGIN(type, tag) \
    virtual void dump(const void* obj, std::ostream& os, int /* unused */) const override {   \
      os << #type <<"{\"" << *(const type*)obj << "\"}";    \
    } \
    virtual void deserialize(void* obj, const rapidxml::xml_node<>* xmlNode) const override { \
      *(type*)obj = (type) std::stod(xmlNode->value()); \
    }  \
    virtual void serialize(const void* obj, rapidxml::xml_node<>* xmlNode, rapidxml::xml_document<>* /* unused */) const override { \
      std::string* str = new std::string(std::to_string(*(type*)obj));  \
      SceneLoader::TrackString(str);  \
      xmlNode->value(str->c_str());  \
    }  \
  __IMPLEMENT_REFLECTION_PRIMITIVE_END(type, tag)

#define IMPLEMENT_REFLECTION_PRIMITIVE_INT(type, tag)	\
  __IMPLEMENT_REFLECTION_PRIMITIVE_BEGIN(type, tag) \
    virtual void dump(const void* obj, std::ostream& os, int /* unused */) const override {   \
      os << #type <<"{\"" << *(const type*)obj << "\"}";    \
    } \
    virtual void deserialize(void* obj, const rapidxml::xml_node<>* xmlNode) const override { \
      *(type*)obj = (type) (int) std::stod(xmlNode->value()); \
    };  \
    virtual void serialize(const void* obj, rapidxml::xml_node<>* xmlNode, rapidxml::xml_document<>* /* unused */) const override { \
      xmlNode->value(std::to_string(*(long double*)obj).c_str());  \
    };  \
  __IMPLEMENT_REFLECTION_PRIMITIVE_END(type, tag)


#define DECLARE_RELFECTION_POINTER(type)  \
  DECLARE_REFLECTION_PRIMITIVE(type)

#define IMPLEMENT_REFLECTION_POINTER(type, refType) \
  void CAT(initReflection_ ## refType, _Ptr)(reflect::TypeDescriptor_Ptr* typeDesc) { \
  typeDesc->name = #type; \
  typeDesc->size = sizeof(type); \
  typeDesc->refTypeGetReflection = refType::GetReflection; \
  } \
  template <> \
  reflect::TypeDescriptor* reflect::getPrimitiveDescriptor<type>() {  \
    static  TypeDescriptor_Ptr typeDesc{CAT(initReflection_ ## refType, _Ptr)};  \
    return &typeDesc; \
  }


DECLARE_REFLECTION_PRIMITIVE(int)
DECLARE_REFLECTION_PRIMITIVE(unsigned int)
DECLARE_REFLECTION_PRIMITIVE(uint64_t)
DECLARE_REFLECTION_PRIMITIVE(float)
DECLARE_REFLECTION_PRIMITIVE(double)
DECLARE_REFLECTION_PRIMITIVE(bool)
DECLARE_REFLECTION_PRIMITIVE(std::string)

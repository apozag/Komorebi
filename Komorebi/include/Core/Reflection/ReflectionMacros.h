#pragma once

#include <iostream>
#include <cstring>
#include <cstddef>

#include "3rd/rapidxml/rapidxml_ext.hpp"
#include "3rd/rapidxml/rapidxml.hpp"

#include "Core/Reflection/TypeDescriptors.h"
#include "Core/Reflection/ReflectionHelper.h"

#define CAT(a, b) a ## b

#define REFLECT_BASE()  \
  __REFLECT(const)

#define REFLECT()  \
  __REFLECT(const override)

#define __REFLECT(X)	\
  friend struct reflection::DefaultResolver;	\
  static const reflection::TypeDescriptor_Struct& GetReflection() { \
    static reflection::TypeDescriptor_Struct Reflection{initReflection}; \
    return Reflection;  \
  } \
  virtual const reflection::TypeDescriptor_Struct& GetReflectionDerived() X{  \
      return GetReflection();  \
  } \
  static void initReflection(reflection::TypeDescriptor_Struct*);	  \

#define __REFLECT_STRUCT_BEGIN(type, parentTypeDescParam, lambdaContent) \
  void type::initReflection(reflection::TypeDescriptor_Struct* typeDesc) { \
    using T = type; \
    typeDesc->name = #type; \
    typeDesc->size = sizeof(T); \
    typeDesc->parentTypeDesc = parentTypeDescParam; \
    typeDesc->construct = [](void* obj){ lambdaContent };  \
    typeDesc->setup = [](void* obj){ if (!std::is_abstract<type>()) (*(type*)obj).Setup(); };  \
    typeDesc->members = {

#define REFLECT_STRUCT_BASE_BEGIN(type) \
  __REFLECT_STRUCT_BEGIN(type, nullptr, new (obj) type();)

#define REFLECT_STRUCT_BASE_VIRTUAL_BEGIN(type) \
  __REFLECT_STRUCT_BEGIN(type, nullptr, )

#define REFLECT_STRUCT_VIRTUAL_BEGIN(type, parentType) \
  __REFLECT_STRUCT_BEGIN(type, &parentType::GetReflection(), )

#define REFLECT_STRUCT_BEGIN(type, parentType) \
  __REFLECT_STRUCT_BEGIN(type, &parentType::GetReflection(), new (obj) type();)

#define REFLECT_STRUCT_MEMBER(name) \
      { #name, reflection::TypeResolver<decltype(T::name)>::get(), [](const void* obj)->void*{return &((T*)obj)->name;}},

#define REFLECT_STRUCT_END(type) \
    }; \
    reflection::ReflectionHelper::RegisterTypeDesc(typeDesc);  \
  } \
  const reflection::TypeDescriptor_Struct& __typeDesc_ ## type = type::GetReflection();

#define DECLARE_REFLECTION_PRIMITIVE(type) \
  namespace reflection {  \
    template <> TypeDescriptor* getPrimitiveDescriptor<type>(); \
  }

#define __IMPLEMENT_REFLECTION_PRIMITIVE_BEGIN(type, tag) \
  struct TypeDescriptor_ ## tag : reflection::TypeDescriptor {  \
    TypeDescriptor_ ## tag() : reflection::TypeDescriptor{ #type, sizeof(type) } {}

#define __IMPLEMENT_REFLECTION_PRIMITIVE_END(type, tag) \
  };  \
  template <> \
  reflection::TypeDescriptor* reflection::getPrimitiveDescriptor<type>() {  \
    static  TypeDescriptor_ ## tag typeDesc;  \
    return &typeDesc; \
  }

#define IMPLEMENT_REFLECTION_PRIMITIVE(type, tag)	\
  __IMPLEMENT_REFLECTION_PRIMITIVE_BEGIN(type, tag) \
    virtual void deserialize(void* obj, const rapidxml::xml_node<>* xmlNode) const override { \
      *(type*)obj = (type) std::stod(xmlNode->value()); \
    }  \
    virtual void serialize(const void* obj, const char* varName, rapidxml::xml_node<>* xmlParent, rapidxml::xml_document<>* doc) const override { \
      rapidxml::xml_node<>* newNode = doc->allocate_node(rapidxml::node_type::node_element, varName); \
      xmlParent->append_node(newNode);  \
      std::string* str = new std::string(std::to_string(*(type*)obj));  \
      reflection::ReflectionHelper::TrackString(str);  \
      newNode->value(str->c_str());  \
    }  \
  __IMPLEMENT_REFLECTION_PRIMITIVE_END(type, tag)

#define DECLARE_REFLECTION_OWNED_POINTER(type)  \
  DECLARE_REFLECTION_PRIMITIVE(reflection::Owned_Ptr_Wrapper<type>)

#define DECLARE_REFLECTION_WEAK_POINTER(type)  \
  DECLARE_REFLECTION_PRIMITIVE(reflection::Weak_Ptr_Wrapper<type>)

#define DECLARE_REFLECTION_POINTER(type)  \
  DECLARE_REFLECTION_OWNED_POINTER(type*) \
  DECLARE_REFLECTION_WEAK_POINTER(type*) 

#define IMPLEMENT_REFLECTION_POINTER(type) \
  void CAT(initReflection_ ## type, _Owned_Ptr)(reflection::TypeDescriptor_Ptr<type>* typeDesc) { \
    typeDesc->name = #type "_Owned_Ptr"; \
    typeDesc->size = sizeof(CAT(type, *)); \
  } \
  void CAT(initReflection_ ## type, _Weak_Ptr)(reflection::TypeDescriptor_Ptr<type>* typeDesc) { \
    typeDesc->name = #type "_Ptr"; \
    typeDesc->size = sizeof(CAT(type, *)); \
  } \
  void CAT(initReflection_ ## type, _Asset_Ptr)(reflection::TypeDescriptor_Ptr<type>* typeDesc) { \
    typeDesc->name = #type "_Asset_Ptr"; \
    typeDesc->size = sizeof(CAT(type, *)); \
  } \
  template <> \
  reflection::TypeDescriptor* reflection::getPrimitiveDescriptor<reflection::Owned_Ptr_Wrapper<type>>() {  \
    static TypeDescriptor_Owned_Ptr<type> typeDesc{CAT(initReflection_ ## type, _Owned_Ptr)};  \
    return &typeDesc; \
  } \
  template <> \
  reflection::TypeDescriptor* reflection::getPrimitiveDescriptor<reflection::Weak_Ptr_Wrapper<type>>() {  \
    static TypeDescriptor_Weak_Ptr<type> typeDesc{CAT(initReflection_ ## type, _Weak_Ptr)};  \
    return &typeDesc; \
  } \
  template <> \
  reflection::TypeDescriptor* reflection::getPrimitiveDescriptor<reflection::Asset_Ptr_Wrapper<type>>() {  \
    static TypeDescriptor_Asset_Ptr<type> typeDesc{CAT(initReflection_ ## type, _Asset_Ptr)};  \
    return &typeDesc; \
  } \
  template <> \
  reflection::TypeDescriptor* reflection::getPrimitiveDescriptor<std::vector<reflection::Owned_Ptr_Wrapper<type>>>() { \
    static TypeDescriptor_StdVector typeDesc{ (reflection::Owned_Ptr_Wrapper<type>*) nullptr };  \
    return &typeDesc; \
  } \
  template <> \
  reflection::TypeDescriptor* reflection::getPrimitiveDescriptor<std::vector<reflection::Weak_Ptr_Wrapper<type>>>() {  \
    static TypeDescriptor_StdVector typeDesc{ (reflection::Weak_Ptr_Wrapper<type>*) nullptr };  \
    return &typeDesc; \
  } \
  template <> \
  reflection::TypeDescriptor* reflection::getPrimitiveDescriptor<std::vector<reflection::Asset_Ptr_Wrapper<type>>>() {  \
    static TypeDescriptor_StdVector typeDesc{ (reflection::Asset_Ptr_Wrapper<type>*) nullptr };  \
    return &typeDesc; \
  } \
  template <> \
  reflection::TypeDescriptor* reflection::getPrimitiveDescriptor<std::vector<type*>>() {  \
    static TypeDescriptor_StdVector typeDesc{ (reflection::Weak_Ptr_Wrapper<type>*) nullptr };  \
    return &typeDesc; \
  }
  

DECLARE_REFLECTION_PRIMITIVE(int)
DECLARE_REFLECTION_PRIMITIVE(unsigned int)
DECLARE_REFLECTION_PRIMITIVE(uint64_t)
DECLARE_REFLECTION_PRIMITIVE(float)
DECLARE_REFLECTION_PRIMITIVE(double)
DECLARE_REFLECTION_PRIMITIVE(bool)
DECLARE_REFLECTION_PRIMITIVE(std::string)

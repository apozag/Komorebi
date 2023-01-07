#pragma once

#include <iostream>
#include <cstring>
#include <cstddef>

#include "rapidxml/rapidxml_ext.hpp"
#include "rapidxml/rapidxml.hpp"

#include "TypeDescriptors.h"
#include "ReflectionHelper.h"

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
  static void initReflection(reflection::TypeDescriptor_Struct*);	

#define REFLECT_STRUCT_BASE_BEGIN(type) \
  void type::initReflection(reflection::TypeDescriptor_Struct* typeDesc) { \
    using T = type; \
    typeDesc->name = #type; \
    typeDesc->size = sizeof(T); \
    typeDesc->members = {

#define REFLECT_STRUCT_BEGIN(type, parentType) \
  void type::initReflection(reflection::TypeDescriptor_Struct* typeDesc) { \
    using T = type; \
    typeDesc->name = #type; \
    typeDesc->size = sizeof(T); \
    typeDesc->parentTypeDesc = &parentType::GetReflection(); \
    typeDesc->members = {

#define REFLECT_STRUCT_MEMBER(name) \
      {#name, offsetof(T, name), reflection::TypeResolver<decltype(T::name)>::get()},

#define REFLECT_STRUCT_END() \
    }; \
    reflection::ReflectionHelper::RegisterTypeDesc(typeDesc);  \
  }

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
  reflection::TypeDescriptor* reflection::getPrimitiveDescriptor<type>() {    \
    static  TypeDescriptor_ ## tag typeDesc;   \
    return &typeDesc;   \
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


#define DECLARE_RELFECTION_POINTER(type)  \
  DECLARE_REFLECTION_PRIMITIVE(type)

#define IMPLEMENT_REFLECTION_POINTER(type, refType) \
  void CAT(initReflection_ ## refType, _Ptr)(reflection::TypeDescriptor_Ptr<refType>* typeDesc) { \
  typeDesc->name = #type; \
  typeDesc->size = sizeof(type); \
  } \
  template <> \
  reflection::TypeDescriptor* reflection::getPrimitiveDescriptor<type>() {  \
    static  TypeDescriptor_Ptr<refType> typeDesc{CAT(initReflection_ ## refType, _Ptr)};  \
    return &typeDesc; \
  }

DECLARE_REFLECTION_PRIMITIVE(int)
DECLARE_REFLECTION_PRIMITIVE(unsigned int)
DECLARE_REFLECTION_PRIMITIVE(uint64_t)
DECLARE_REFLECTION_PRIMITIVE(float)
DECLARE_REFLECTION_PRIMITIVE(double)
DECLARE_REFLECTION_PRIMITIVE(bool)
DECLARE_REFLECTION_PRIMITIVE(std::string)

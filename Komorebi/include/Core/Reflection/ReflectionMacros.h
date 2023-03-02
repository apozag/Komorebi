#pragma once

#include <iostream>
#include <cstring>
#include <cstddef>

#include "3rd/rapidxml/rapidxml_ext.hpp"
#include "3rd/rapidxml/rapidxml.hpp"

#include "Core/Reflection/TypeDescriptors.h"
#include "Core/Reflection/ReflectionHelper.h"

#define CAT(a, b) a ## b

#define __REFLECT(X)	\
  friend struct reflection::DefaultResolver;	\
  static const reflection::TypeDescriptor_Struct& GetReflection() { \
    static reflection::TypeDescriptor_Struct Reflection{initReflection}; \
    return Reflection;  \
  } \
  virtual const reflection::TypeDescriptor_Struct* GetReflectionDynamic() X{  \
      return &GetReflection();  \
  } \
  static void initReflection(reflection::TypeDescriptor_Struct*);	

#define REFLECT_BASE()  \
  __REFLECT(const)

#define REFLECT()  \
  __REFLECT(const override)

#define REFLECT_HIDE()  \
    friend struct reflection::DefaultResolver;	\
  static const reflection::TypeDescriptor_Ignored& GetReflection() { \
    static reflection::TypeDescriptor_Ignored Reflection; \
    return Reflection;  \
  } \
  virtual const reflection::TypeDescriptor_Struct* GetReflectionDynamic() const override{  \
      return &GetReflection();  \
  } \
  static void initReflection(reflection::TypeDescriptor_Struct*);

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
      { #name, reflection::TypeResolver<decltype(T::name)>::get(), [](const void* obj)->void*{return &(((T*)obj)->name);}},

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
    std::string GetValueStr(const void* obj) const override { \
      return std::to_string(*(type*)obj); \
    } \
    void SetValueFromString(void* pObj, const char* valueCStr) const override { \
      *(type*)pObj = (type)std::stod(valueCStr); \
    } \
  __IMPLEMENT_REFLECTION_PRIMITIVE_END(type, tag)

#define DECLARE_REFLECTION_OWNED_POINTER(type)  \
  DECLARE_REFLECTION_PRIMITIVE(reflection::Owned_Ptr_Wrapper<type>)

#define DECLARE_REFLECTION_WEAK_POINTER(type)  \
  DECLARE_REFLECTION_PRIMITIVE(reflection::Weak_Ptr_Wrapper<type>)

#define DECLARE_REFLECTION_POINTER(type)  \
  DECLARE_REFLECTION_OWNED_POINTER(type*) \
  DECLARE_REFLECTION_WEAK_POINTER(type*) 

#define IMPLEMENT_REFLECTION_POINTER(type) \
  void CAT(initReflection_ ## type, _Owned_Ptr)(reflection::TypeDescriptor_Ptr* typeDesc) { \
    typeDesc->name = #type "_Owned_Ptr"; \
    typeDesc->size = sizeof(CAT(type, *)); \
    typeDesc->getDynamicType =  [](const void* pObj)->const reflection::TypeDescriptor*{  \
      return pObj ? reflection::TypeResolver<type>::getDynamic(pObj) : reflection::TypeResolver<type>::get();  \
    }; \
  } \
  void CAT(initReflection_ ## type, _Weak_Ptr)(reflection::TypeDescriptor_Ptr* typeDesc) { \
    typeDesc->name = #type "_Ptr"; \
    typeDesc->size = sizeof(CAT(type, *)); \
    typeDesc->getDynamicType =  [](const void* pObj)->const reflection::TypeDescriptor*{  \
      return pObj ? reflection::TypeResolver<type>::getDynamic(pObj) : reflection::TypeResolver<type>::get();  \
    }; \
  } \
  template <> \
  reflection::TypeDescriptor* reflection::getPrimitiveDescriptor<reflection::Owned_Ptr_Wrapper<type>>() {  \
    static TypeDescriptor_Owned_Ptr typeDesc{CAT(initReflection_ ## type, _Owned_Ptr)};  \
    return &typeDesc; \
  } \
  template <> \
  reflection::TypeDescriptor* reflection::getPrimitiveDescriptor<reflection::Weak_Ptr_Wrapper<type>>() {  \
    static TypeDescriptor_Weak_Ptr typeDesc{CAT(initReflection_ ## type, _Weak_Ptr)};  \
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
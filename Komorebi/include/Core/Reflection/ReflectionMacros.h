#pragma once

#include <iostream>
#include <cstring>
#include <cstddef>

#include "3rd/rapidxml/rapidxml_ext.hpp"
#include "3rd/rapidxml/rapidxml.hpp"

#include "Core/Reflection/TypeDescriptors.h"
#include "Core/Reflection/ReflectionHelper.h"

#include "Core/Memory/Factory.h"

#define CAT(a, b) a ## b

#define __REFLECT(X)	\
  friend struct ::reflection::DefaultResolver;	\
  static const ::reflection::TypeDescriptor_Struct& GetReflection() { \
    static ::reflection::TypeDescriptor_Struct Reflection{initReflection}; \
    return Reflection;  \
  } \
  virtual const ::reflection::TypeDescriptor_Struct* GetReflectionDynamic() const X{  \
      return &GetReflection();  \
  } \
  static void initReflection(::reflection::TypeDescriptor_Struct*);	

#define REFLECT_BASE()  \
  __REFLECT()

#define REFLECT()  \
  __REFLECT(override)

#define REFLECT_HIDE()  \
    friend struct ::reflection::DefaultResolver;	\
  static const ::reflection::TypeDescriptor_Ignored& GetReflection() { \
    static ::reflection::TypeDescriptor_Ignored Reflection; \
    return Reflection;  \
  } \
  virtual const ::reflection::TypeDescriptor_Struct* GetReflectionDynamic() const override{  \
      return &GetReflection();  \
  } \
  static void initReflection(::reflection::TypeDescriptor_Struct*);

#define __REFLECT_STRUCT_BEGIN(TYPE, PARENT_TYPE_EXP, CONSTRUCT_EXP, CREATE_EXP) \
  void TYPE::initReflection(::reflection::TypeDescriptor_Struct* typeDesc) { \
    using T = TYPE; \
    typeDesc->name = #TYPE; \
    typeDesc->size = sizeof(T); \
    typeDesc->parentTypeDesc = PARENT_TYPE_EXP; \
    typeDesc->construct = [](void* obj){ CONSTRUCT_EXP };  \
    typeDesc->create = []()->void*{ CREATE_EXP };  \
    typeDesc->setup = [](void* obj){ if (!std::is_abstract<TYPE>()) (*(T*)obj).Setup(); };  \
    typeDesc->reconfigure = [](void* obj){ if (!std::is_abstract<TYPE>()) (*(T*)obj).Reconfigure(); };  \
    typeDesc->members = {

#define REFLECT_STRUCT_BASE_BEGIN(TYPE) \
  __REFLECT_STRUCT_BEGIN(TYPE, nullptr, new (obj) TYPE();, return (void*)memory::Factory::Create<TYPE>();)

#define REFLECT_STRUCT_BASE_VIRTUAL_BEGIN(TYPE) \
  __REFLECT_STRUCT_BEGIN(TYPE, nullptr, , return nullptr;)

#define REFLECT_STRUCT_VIRTUAL_BEGIN(TYPE, PARENT_TYPE) \
  __REFLECT_STRUCT_BEGIN(TYPE, &PARENT_TYPE::GetReflection(), , return nullptr;)

#define REFLECT_STRUCT_BEGIN(TYPE, PARENT_TYPE) \
  __REFLECT_STRUCT_BEGIN(TYPE, &PARENT_TYPE::GetReflection(), new (obj) TYPE();, return (void*)memory::Factory::Create<TYPE>();)

#define REFLECT_STRUCT_MEMBER(NAME) \
      { #NAME, ::reflection::TypeResolver<decltype(T::NAME)>::get(), [](const void* obj)->void*{return &(((T*)obj)->NAME);}},

#define REFLECT_STRUCT_END(TYPE) \
    }; \
    ::reflection::ReflectionHelper::RegisterTypeDesc(typeDesc);  \
  } \
  const ::reflection::TypeDescriptor_Struct& __typeDesc_ ## TYPE = TYPE::GetReflection(); \

#define DECLARE_REFLECTION_PRIMITIVE(TYPE) \
  namespace reflection {  \
    template <> TypeDescriptor* getPrimitiveDescriptor<TYPE>(); \
  }

#define __IMPLEMENT_REFLECTION_PRIMITIVE_BEGIN(TYPE, TAG) \
  struct TypeDescriptor_ ## TAG : ::reflection::TypeDescriptor {  \
    TypeDescriptor_ ## TAG() : ::reflection::TypeDescriptor{ #TYPE, sizeof(TYPE) } {}

#define __IMPLEMENT_REFLECTION_PRIMITIVE_END(TYPE, TAG) \
  };  \
  template <> \
  ::reflection::TypeDescriptor* ::reflection::getPrimitiveDescriptor<TYPE>() {  \
    static  TypeDescriptor_ ## TAG typeDesc;  \
    return &typeDesc; \
  }

#define IMPLEMENT_REFLECTION_PRIMITIVE(TYPE, TAG)	\
  __IMPLEMENT_REFLECTION_PRIMITIVE_BEGIN(TYPE, TAG) \
    std::string GetValueStr(const void* obj) const override { \
      return std::to_string(*(TYPE*)obj); \
    } \
    void SetValueFromString(void* pObj, const char* valueCStr) const override { \
      *(TYPE*)pObj = (TYPE)std::stod(valueCStr); \
    } \
  __IMPLEMENT_REFLECTION_PRIMITIVE_END(TYPE, TAG)

#define DECLARE_REFLECTION_OWNED_POINTER(TYPE)  \
  DECLARE_REFLECTION_PRIMITIVE(::reflection::Owned_Ptr_Wrapper<TYPE>)

#define DECLARE_REFLECTION_WEAK_POINTER(TYPE)  \
  DECLARE_REFLECTION_PRIMITIVE(::reflection::Weak_Ptr_Wrapper<TYPE>)

#define DECLARE_REFLECTION_POINTER(TYPE)  \
  DECLARE_REFLECTION_OWNED_POINTER(TYPE*) \
  DECLARE_REFLECTION_WEAK_POINTER(TYPE*) 

#define __IMPLEMENT_REFLECTION_POINTER(TYPE, NAMESPACE) \
  void CAT(initReflection_ ## TYPE, _Owned_Ptr)(::reflection::TypeDescriptor_Ptr* typeDesc) { \
    typeDesc->name = #TYPE "_Owned_Ptr"; \
    typeDesc->size = sizeof(CAT(NAMESPACE ## TYPE, *)); \
    typeDesc->getDynamicType =  [](const void* pObj)->const ::reflection::TypeDescriptor*{  \
      return pObj ? ::reflection::TypeResolver<NAMESPACE ## TYPE>::getDynamic(pObj) : ::reflection::TypeResolver<NAMESPACE ## TYPE>::get();  \
    }; \
    typeDesc->getStaticType = []() -> const ::reflection::TypeDescriptor*{  \
      return ::reflection::TypeResolver<NAMESPACE ## TYPE>::get(); \
    };  \
  } \
  void CAT(initReflection_ ## TYPE, _Weak_Ptr)(::reflection::TypeDescriptor_Ptr* typeDesc) { \
    typeDesc->name = #NAMESPACE #TYPE "_Ptr"; \
    typeDesc->size = sizeof(CAT(NAMESPACE ## TYPE, *)); \
    typeDesc->getDynamicType =  [](const void* pObj)->const ::reflection::TypeDescriptor*{  \
      return pObj ? ::reflection::TypeResolver<NAMESPACE ## TYPE>::getDynamic(pObj) : ::reflection::TypeResolver<NAMESPACE ## TYPE>::get();  \
    }; \
    typeDesc->getStaticType = []() -> const ::reflection::TypeDescriptor*{  \
      return ::reflection::TypeResolver<NAMESPACE ## TYPE>::get(); \
    };  \
  } \
  template <> \
  ::reflection::TypeDescriptor* ::reflection::getPrimitiveDescriptor<::reflection::Owned_Ptr_Wrapper<NAMESPACE ## TYPE>>() {  \
    static TypeDescriptor_Owned_Ptr typeDesc{CAT(initReflection_ ## TYPE, _Owned_Ptr)};  \
    return &typeDesc; \
  } \
  template <> \
  ::reflection::TypeDescriptor* ::reflection::getPrimitiveDescriptor<::reflection::Weak_Ptr_Wrapper<NAMESPACE ## TYPE>>() {  \
    static TypeDescriptor_Weak_Ptr typeDesc{CAT(initReflection_ ## TYPE, _Weak_Ptr)};  \
    return &typeDesc; \
  } \
  template <> \
  ::reflection::TypeDescriptor* ::reflection::getPrimitiveDescriptor<std::vector<::reflection::Owned_Ptr_Wrapper<NAMESPACE ## TYPE>>>() { \
    static TypeDescriptor_StdVector typeDesc{ (::reflection::Owned_Ptr_Wrapper<NAMESPACE ## TYPE>*) nullptr };  \
    return &typeDesc; \
  } \
  template <> \
  ::reflection::TypeDescriptor* ::reflection::getPrimitiveDescriptor<std::vector<::reflection::Weak_Ptr_Wrapper<NAMESPACE ## TYPE>>>() {  \
    static TypeDescriptor_StdVector typeDesc{ (::reflection::Weak_Ptr_Wrapper<NAMESPACE ## TYPE>*) nullptr };  \
    return &typeDesc; \
  } \
  template <> \
  ::reflection::TypeDescriptor* ::reflection::getPrimitiveDescriptor<std::vector<NAMESPACE ## TYPE*>>() {  \
    static TypeDescriptor_StdVector typeDesc{ (::reflection::Weak_Ptr_Wrapper<NAMESPACE ## TYPE>*) nullptr };  \
    return &typeDesc; \
  }

#define IMPLEMENT_REFLECTION_POINTER_NAMESPACE(NAMESPACE, TYPE) __IMPLEMENT_REFLECTION_POINTER(TYPE, NAMESPACE::)
#define IMPLEMENT_REFLECTION_POINTER(TYPE) __IMPLEMENT_REFLECTION_POINTER(TYPE,::)  

DECLARE_REFLECTION_PRIMITIVE(int)
DECLARE_REFLECTION_PRIMITIVE(unsigned int)
DECLARE_REFLECTION_PRIMITIVE(uint64_t)
DECLARE_REFLECTION_PRIMITIVE(float)
DECLARE_REFLECTION_PRIMITIVE(double)
DECLARE_REFLECTION_PRIMITIVE(bool)
DECLARE_REFLECTION_PRIMITIVE(std::string)
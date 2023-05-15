#pragma once

#include <iostream>
#include <cstring>
#include <cstddef>

#include "3rd/rapidxml/rapidxml_ext.hpp"
#include "3rd/rapidxml/rapidxml.hpp"

#include "Core/PrefabManager.h"
#include "Core/Reflection/TypeDescriptors.h"
#include "Core/Reflection/ReflectionHelper.h"

#include "Core/Memory/Factory.h"

#define CAT(a, b) a ## b

////////////////////////////////////////////////////////////////////////
// Struct / Class
////////////////////////////////////////////////////////////////////////

#define __REFLECT_STRUCT_BEGIN(TYPE, PARENT_TYPE_EXP, CONSTRUCT_EXP, CREATE_EXP, DESTROY_EXP) \
  void TYPE::initReflection(::reflection::TypeDescriptor_Struct* typeDesc) { \
    using T = TYPE; \
    typeDesc->name = #TYPE; \
    typeDesc->size = sizeof(T); \
    typeDesc->parentTypeDesc = PARENT_TYPE_EXP; \
    typeDesc->construct = [](void* obj){ CONSTRUCT_EXP };  \
    typeDesc->create = []()->void*{ CREATE_EXP };  \
    typeDesc->destroy = [](void* obj){ DESTROY_EXP };  \
    typeDesc->setup = [](void* obj){ if (!std::is_abstract<TYPE>()) (*(T*)obj).Setup(); };  \
    typeDesc->reconfigure = [](void* obj){ if (!std::is_abstract<TYPE>()) (*(T*)obj).Reconfigure(); };  \
    typeDesc->members = {

#define REFLECT_STRUCT_BASE_BEGIN(TYPE) \
  __REFLECT_STRUCT_BEGIN(TYPE, nullptr, new (obj) TYPE();, return (void*)memory::Factory::Create<TYPE>();, memory::Factory::Destroy((TYPE*)obj);)

#define REFLECT_STRUCT_BASE_VIRTUAL_BEGIN(TYPE) \
  __REFLECT_STRUCT_BEGIN(TYPE, nullptr, , return nullptr;, )

#define REFLECT_STRUCT_VIRTUAL_BEGIN(TYPE, PARENT_TYPE) \
  __REFLECT_STRUCT_BEGIN(TYPE, &PARENT_TYPE::GetReflection(), , return nullptr;, )

#define REFLECT_STRUCT_BEGIN(TYPE, PARENT_TYPE) \
  __REFLECT_STRUCT_BEGIN(TYPE, &PARENT_TYPE::GetReflection(), new (obj) TYPE();, return (void*)memory::Factory::Create<TYPE>();, memory::Factory::Destroy((TYPE*)obj);)

#define REFLECT_STRUCT_MEMBER(NAME) \
      { #NAME, ::reflection::TypeResolver<decltype(T::NAME)>::get(), [](const void* obj)->void*{return &(((T*)obj)->NAME);}},

#define REFLECT_STRUCT_END(TYPE) \
    }; \
    ::reflection::ReflectionHelper::RegisterTypeDesc(typeDesc);  \
  } \
  const ::reflection::TypeDescriptor_Struct& __typeDesc_ ## TYPE = TYPE::GetReflection(); \

////////////////////////////////////////////////////////////////////////
// Primitive
////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////
// Enum
////////////////////////////////////////////////////////////////////////

#define REFLECT_ENUM_BEGIN(NAME) \
  namespace CAT(__, NAME ## EnumReflectionInternal){  \
    void initReflection(::reflection::TypeDescriptor_Enum* typeDesc) { \
      using T = NAME; \
      typeDesc->name = #NAME; \
      typeDesc->size = sizeof(T); \
      typeDesc->values = {

#define REFLECT_ENUM_VALUE(VALUE)  \
        ::reflection::TypeDescriptor_Enum::EnumValue{#VALUE, (int) T::VALUE},

#define REFLECT_ENUM_END(NAME)  \
      };  \
      ::reflection::ReflectionHelper::RegisterTypeDesc(typeDesc);  \
    } \
  } \
  template <> \
  ::reflection::TypeDescriptor* ::reflection::getPrimitiveDescriptor<NAME>() {  \
    static TypeDescriptor_Enum typeDesc {CAT(__, NAME ## EnumReflectionInternal)::initReflection};  \
    return &typeDesc; \
  } \
  const ::reflection::TypeDescriptor* CAT(__typeDesc_ ## NAME, _Enum) = ::reflection::TypeResolver<NAME>::get(); \

////////////////////////////////////////////////////////////////////////
// Enum Bitmask
////////////////////////////////////////////////////////////////////////

#define IMPLEMENT_REFLECTION_BITMASK(NAME) \
  namespace CAT(__, NAME ## BitmaskReflectionInternal){  \
    void initReflection(::reflection::TypeDescriptor_Bitmask* typeDesc) { \
      using T = NAME; \
      typeDesc->name = #NAME "Bitmask"; \
      typeDesc->size = sizeof(T); \
      typeDesc->enumType = (::reflection::TypeDescriptor_Enum*) ::reflection::TypeResolver<NAME>::get(); \
      ::reflection::ReflectionHelper::RegisterTypeDesc(typeDesc);  \
    } \
  } \
  template <> \
  ::reflection::TypeDescriptor* ::reflection::getPrimitiveDescriptor<::reflection::BitmaskWrapper<NAME>>() {  \
    static TypeDescriptor_Bitmask typeDesc {CAT(__, NAME ## BitmaskReflectionInternal)::initReflection};  \
    return &typeDesc; \
  } \
  const ::reflection::TypeDescriptor* CAT(__typeDesc_ ## NAME, _Bitmask) = ::reflection::TypeResolver<::reflection::BitmaskWrapper<NAME>>::get(); \

////////////////////////////////////////////////////////////////////////
// Pointer
////////////////////////////////////////////////////////////////////////

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
  ::reflection::TypeDescriptor* ::reflection::getPrimitiveDescriptor<NAMESPACE ## TYPE *>() {  \
    return getPrimitiveDescriptor<Weak_Ptr_Wrapper<NAMESPACE ## TYPE>>(); \
  } 

#define __IMPLEMENT_REFLECTION_ASSET_POINTER(TYPE, NAMESPACE) \
  void CAT(initReflection_ ## TYPE, _Asset_Ptr)(::reflection::TypeDescriptor_Ptr* typeDesc) { \
    typeDesc->name = #TYPE "_Asset_Ptr"; \
    typeDesc->size = sizeof(CAT(NAMESPACE ## TYPE, *)); \
    typeDesc->getDynamicType = [](const void* pObj)->const ::reflection::TypeDescriptor* {  \
      return pObj ? ::reflection::TypeResolver<NAMESPACE ## TYPE>::getDynamic(pObj) : ::reflection::TypeResolver<NAMESPACE ## TYPE>::get();  \
    }; \
    typeDesc->getStaticType = []() -> const ::reflection::TypeDescriptor* {  \
      return ::reflection::TypeResolver<NAMESPACE ## TYPE>::get(); \
    };  \
    ((::reflection::TypeDescriptor_Asset_Ptr*)typeDesc)->getFilename = [](const void* pObj)-> std::string { \
      return ((::reflection::Asset_Ptr_Wrapper<NAMESPACE ## TYPE>*)pObj)->m_filename; \
    };  \
    ((::reflection::TypeDescriptor_Asset_Ptr*)typeDesc)->setFilename = [](void* pObj, const std::string& str) { \
      ((::reflection::Asset_Ptr_Wrapper<NAMESPACE ## TYPE>*)pObj)->m_filename = str; \
    };  \
    ((::reflection::TypeDescriptor_Asset_Ptr*)typeDesc)->loadPrefab = [](void* pObj) { \
      ::reflection::Asset_Ptr_Wrapper<NAMESPACE ## TYPE>* pAssetPtr = ((::reflection::Asset_Ptr_Wrapper<NAMESPACE ## TYPE>*)pObj);  \
      pAssetPtr->m_ptr = PrefabManager::GetInstance()->LoadPrefab<NAMESPACE ## TYPE>(pAssetPtr->m_filename.c_str()); \
    };  \
    ((::reflection::TypeDescriptor_Asset_Ptr*)typeDesc)->getPPtr = [](void* pObj)-> void ** { \
      return (void**)&(((::reflection::Ptr_Wrapper<NAMESPACE ## TYPE>*)pObj)->m_ptr); \
    };  \
  } \
  template <> \
  ::reflection::TypeDescriptor* ::reflection::getPrimitiveDescriptor<::reflection::Asset_Ptr_Wrapper<NAMESPACE ## TYPE>>() { \
    static TypeDescriptor_Asset_Ptr typeDesc{ CAT(initReflection_ ## TYPE, _Asset_Ptr) };  \
    return &typeDesc; \
  }

#define IMPLEMENT_REFLECTION_POINTER_NAMESPACE(NAMESPACE, TYPE) __IMPLEMENT_REFLECTION_POINTER(TYPE, NAMESPACE::)
#define IMPLEMENT_REFLECTION_POINTER(TYPE) __IMPLEMENT_REFLECTION_POINTER(TYPE,::)  

#define IMPLEMENT_REFLECTION_ASSET_POINTER_NAMESPACE(NAMESPACE, TYPE) __IMPLEMENT_REFLECTION_ASSET_POINTER(TYPE, NAMESPACE::)
#define IMPLEMENT_REFLECTION_ASSET_POINTER(TYPE) __IMPLEMENT_REFLECTION_ASSET_POINTER(TYPE,::)  

////////////////////////////////////////////////////////////////////////
// Vector
////////////////////////////////////////////////////////////////////////

#define IMPLEMENT_REFLECTION_VECTOR(TYPE) \
  template <> \
  reflection::TypeDescriptor* reflection::getPrimitiveDescriptor<std::vector<TYPE>>() {  \
    static TypeDescriptor_StdVector typeDesc{ (TYPE*) nullptr };  \
    return &typeDesc; \
  }

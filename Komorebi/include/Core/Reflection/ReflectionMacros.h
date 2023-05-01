#pragma once

#include "Core/Reflection/TypeDescriptors.h"

#pragma warning (disable: 4003)

#define CAT(a, b) a ## b

////////////////////////////////////////////////////////////////////////
// Struct / Class
////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////
// Primitive
////////////////////////////////////////////////////////////////////////

#define DECLARE_REFLECTION_PRIMITIVE(TYPE) \
  namespace reflection {  \
    template <> TypeDescriptor* getPrimitiveDescriptor<TYPE>(); \
  }

DECLARE_REFLECTION_PRIMITIVE(int)
DECLARE_REFLECTION_PRIMITIVE(unsigned int)
DECLARE_REFLECTION_PRIMITIVE(uint64_t)
DECLARE_REFLECTION_PRIMITIVE(float)
DECLARE_REFLECTION_PRIMITIVE(double)
DECLARE_REFLECTION_PRIMITIVE(bool)
DECLARE_REFLECTION_PRIMITIVE(std::string)

////////////////////////////////////////////////////////////////////////
// Enum
////////////////////////////////////////////////////////////////////////

#define DECLARE_REFLECTION_ENUM(NAME) \
  DECLARE_REFLECTION_PRIMITIVE(NAME)

////////////////////////////////////////////////////////////////////////
// Enum Bitmask
////////////////////////////////////////////////////////////////////////

#define DECLARE_REFLECTION_BITMASK(NAME) \
  DECLARE_REFLECTION_PRIMITIVE(::reflection::BitmaskWrapper<NAME>)

////////////////////////////////////////////////////////////////////////
// Pointer
////////////////////////////////////////////////////////////////////////

#define DECLARE_REFLECTION_OWNED_POINTER(TYPE)  \
  DECLARE_REFLECTION_PRIMITIVE(::reflection::Owned_Ptr_Wrapper<TYPE>)

#define DECLARE_REFLECTION_WEAK_POINTER(TYPE)  \
  DECLARE_REFLECTION_PRIMITIVE(::reflection::Weak_Ptr_Wrapper<TYPE>)

#define DECLARE_REFLECTION_ASSET_POINTER(TYPE)  \
DECLARE_REFLECTION_PRIMITIVE(::reflection::Asset_Ptr_Wrapper<TYPE>)

#define DECLARE_REFLECTION_POINTER(TYPE)  \
  DECLARE_REFLECTION_OWNED_POINTER(TYPE) \
  DECLARE_REFLECTION_WEAK_POINTER(TYPE)  \
  DECLARE_REFLECTION_PRIMITIVE(TYPE*)

////////////////////////////////////////////////////////////////////////
// Vector
////////////////////////////////////////////////////////////////////////

#define DECLARE_REFLECTION_VECTOR(TYPE) \
  DECLARE_REFLECTION_PRIMITIVE(std::vector<TYPE>)

DECLARE_REFLECTION_VECTOR(std::string)


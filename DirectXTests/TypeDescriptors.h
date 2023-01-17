#pragma once
#include <cstddef>
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

  template <typename T>
  struct Ptr_Wrapper {
    T* m_ptr;

    T* operator->() const { return m_ptr; }
    operator T* () const { return m_ptr; }
    const T& operator* () const {return *m_ptr;}
    T& operator* () {return *m_ptr;}
    T* operator+(int offset) { return m_ptr + offset; }
    T* operator++() { return m_ptr++; }
    T* operator-(int offset) { return m_ptr - offset; }
    T* operator--() { return m_ptr--; }
  protected:
    Ptr_Wrapper() {}
    Ptr_Wrapper(T* ptr) : m_ptr(ptr) {}
  };
  template <typename T>
  struct Owned_Ptr_Wrapper: public Ptr_Wrapper<T> {
    Owned_Ptr_Wrapper() {}
    Owned_Ptr_Wrapper(T* ptr) : Ptr_Wrapper<T>(ptr) {}
  };
  template <typename T>
  struct Weak_Ptr_Wrapper : public Ptr_Wrapper<T> {
    Weak_Ptr_Wrapper() {}
    Weak_Ptr_Wrapper(T* ptr) : Ptr_Wrapper<T>(ptr) {}
  };

#define WEAK_PTR(TYPE) reflection::Weak_Ptr_Wrapper<TYPE>
#define OWNED_PTR(TYPE) reflection::Owned_Ptr_Wrapper<TYPE>

  template <typename T>
  TypeDescriptor* getPrimitiveDescriptor();

  struct DefaultResolver {
    template <typename T> static char func(decltype(&T::GetReflection));
    template <typename T> static int func(...);
    template <typename T>
    struct IsReflected {
      enum { value = (sizeof(func<T>(nullptr)) == sizeof(char)) };
    };

    template <typename T, typename std::enable_if<IsReflected<T>::value, int>::type = 0>
    static const TypeDescriptor* get() {
      return &T::GetReflection();
    }

    template <typename T, typename std::enable_if<!IsReflected<T>::value && !std::is_pointer<T>::value, int>::type = 0>
    static const TypeDescriptor* get() {
      return getPrimitiveDescriptor<T>();
    }

    template <typename T, typename std::enable_if<!IsReflected<T>::value && std::is_pointer<T>::value, int>::type = 0>
    static const TypeDescriptor* get() {
      return getPrimitiveDescriptor<WEAK_PTR(std::remove_pointer<T>::type)>();
    }
  };

  template <typename T, typename Enable = void>
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
    virtual void deserialize(void* obj, const rapidxml::xml_node<>* xmlNode) const {}
    virtual void serialize(const void* obj, const char* varName, rapidxml::xml_node<>* xmlParent, rapidxml::xml_document<>* doc) const {}
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

    const TypeDescriptor_Struct* parentTypeDesc;

    void (*setup)(void*);

    TypeDescriptor_Struct(void (*init)(TypeDescriptor_Struct*)) : TypeDescriptor{ nullptr, 0 }
    {
      init(this);
    }
    TypeDescriptor_Struct(const char* name, size_t size, const std::initializer_list<Member>& init) : TypeDescriptor{ nullptr, 0 }, members{ init }
    {}

    virtual void deserialize(void* obj, const rapidxml::xml_node<>* xmlNode) const override;

    virtual void serialize(const void* obj, const char* varName, rapidxml::xml_node<>* xmlParent, rapidxml::xml_document<>* doc) const override;

  protected:
    void serializeMembers(const void* obj, rapidxml::xml_node<>* xmlNode, rapidxml::xml_document<>* doc) const;

    int getFirstMemberIdx() const;
  };

  //--------------------------------------------------------
  // Type descriptor for pointer to struct/class
  //--------------------------------------------------------

  constexpr const char* __PtrIdAttName = "PtrId";
  constexpr const char* __PtrTypeAttName = "Type";

  template <typename T>
  struct TypeDescriptor_Ptr : public TypeDescriptor {

    TypeDescriptor_Ptr(void (*init)(TypeDescriptor_Ptr*)) : TypeDescriptor{ nullptr, 0 }
    {
      init(this);
    }
    TypeDescriptor_Ptr(const char* name, size_t size) : TypeDescriptor{ nullptr, 0 }
    {}
  private:
    template <typename T> static char func(decltype(&T::GetReflectionDerived));
    template <typename T> static int func(...);
    template <typename T>
    struct IsReflected {
      enum { value = (sizeof(func<T>(nullptr)) == sizeof(char)) };
    };
  public:

    template <typename T, typename std::enable_if<IsReflected<T>::value, int>::type = 0>
    static const TypeDescriptor* getReferencedType(const void* obj) {
      return &(*(T**)obj)->GetReflectionDerived();
    }

    template <typename T, typename std::enable_if<!IsReflected<T>::value, int>::type = 0>
    static const TypeDescriptor* getReferencedType(const void* /* unused */) {
      return getPrimitiveDescriptor<T>();
    }
  };

  template <typename T>
  struct TypeDescriptor_Owned_Ptr : public TypeDescriptor_Ptr<T> {

    struct Ptr_Wrapper {
      T* m_ptr;
      Ptr_Wrapper(T* ptr) : m_ptr(ptr) {}
      T* operator->() const {return m_ptr;}
      operator T* () const { return m_ptr; }
    };

    using TypeDescriptor_Ptr<T>::getReferencedType;

    TypeDescriptor_Owned_Ptr(void (*init)(TypeDescriptor_Ptr<T>*)) : TypeDescriptor_Ptr<T>{ init }
    {}
    TypeDescriptor_Owned_Ptr(const char* name, size_t size) : TypeDescriptor{ nullptr, 0 } 
    {}

    virtual void deserialize(void* obj, const rapidxml::xml_node<>* xmlNode) const override {
      void** ppObj = (void**)obj;
      char* name = nullptr;
      unsigned int ptrId = 0xFFFFFFFF;

      rapidxml::xml_attribute<>* att = xmlNode->first_attribute();
      while (att) {
        if (strcmp(att->name(), __PtrIdAttName) == 0) {
          ptrId = (unsigned int)std::atoi(att->value());
        }
        else if (strcmp(att->name(), __PtrTypeAttName) == 0) {
          name = att->value();
        }
        att = att->next_attribute();
      }

      if (!name) {
        // TODO: [ERROR] Owned Pointer node must have a "Type" attribute
      }
      else if (ptrId == 0xFFFFFFFF) {
        // TODO: [ERROR] Owned Pointer node must have a "PrtId" attribute
      }

      const TypeDescriptor* typeDesc = ReflectionHelper::GetTypeDesc(name);

      *ppObj = malloc(typeDesc->size);

      ReflectionHelper::RegisterPtrId(*ppObj, ptrId, typeDesc->size);

      typeDesc->deserialize(*ppObj, xmlNode);
    };

    virtual void serialize(const void* obj, const char* varName, rapidxml::xml_node<>* xmlParent, rapidxml::xml_document<>* doc) const override {
      const void** ppObj = (const void**)obj;

      const TypeDescriptor* typeDesc = (*ppObj) ? getReferencedType<T>(obj) : TypeResolver<T>::get();

      if (!varName) {
        std::string* name = new std::string(typeDesc->getFullName() + "*");
        ReflectionHelper::TrackString(name);
        varName = name->c_str();
      }

      rapidxml::xml_node<>* firstChild = xmlParent->first_node();

      typeDesc->serialize(*ppObj, varName, xmlParent, doc);

      rapidxml::xml_node<>* newNode = firstChild ? firstChild->next_sibling() : xmlParent->first_node();
      std::string* str = new std::string(std::to_string((unsigned int)*ppObj));
      ReflectionHelper::TrackString(str);
      rapidxml::xml_attribute<>* idAtt = doc->allocate_attribute(__PtrIdAttName, str->c_str());
      newNode->append_attribute(idAtt);

      std::string* typeName = new std::string(typeDesc->getFullName());
      ReflectionHelper::TrackString(typeName);
      rapidxml::xml_attribute<>* typeAtt = doc->allocate_attribute(__PtrTypeAttName, typeName->c_str());
      newNode->append_attribute(typeAtt);
    }
  };

  template <typename T>
  struct TypeDescriptor_Weak_Ptr : public TypeDescriptor_Ptr<T> {

    using TypeDescriptor_Ptr<T>::getReferencedType;

    TypeDescriptor_Weak_Ptr(void (*init)(TypeDescriptor_Ptr<T>*)) : TypeDescriptor_Ptr<T>{ init }
    {}
    TypeDescriptor_Weak_Ptr(const char* name, size_t size) : TypeDescriptor{ nullptr, 0 }
    {}

    virtual void deserialize(void* obj, const rapidxml::xml_node<>* xmlNode) const override {
      ReflectionHelper::RegisterPendingPtr((void**)obj, (unsigned int)std::atoi(xmlNode->value()));
    };

    virtual void serialize(const void* obj, const char* varName, rapidxml::xml_node<>* xmlParent, rapidxml::xml_document<>* doc) const override {
      const void** ppObj = (const void**)obj;

      const TypeDescriptor* typeDesc = (*ppObj)? getReferencedType<T>(obj) : TypeResolver<T>::get();
      if (!varName) {
        std::string* name = new std::string(typeDesc->getFullName() + "*");
        ReflectionHelper::TrackString(name);
        varName = name->c_str();
      }
      rapidxml::xml_node<>* newNode = doc->allocate_node(rapidxml::node_type::node_element, varName);
      xmlParent->append_node(newNode);
      std::string* str = new std::string(std::to_string((unsigned int)*ppObj));
      ReflectionHelper::TrackString(str);
      newNode->value(str->c_str());
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

  template <typename T>
  class TypeResolver<std::vector<T>, typename std::enable_if_t<!std::is_pointer<T>::value, int>::type> {
  public:
    static TypeDescriptor* get() {
      static TypeDescriptor_StdVector typeDesc{ (T*) nullptr };
      return &typeDesc;
    }
  };

  template <typename T>
  class TypeResolver<std::vector<T>, typename std::enable_if_t<std::is_pointer<T>::value, int>::type> {
  public:
    static TypeDescriptor* get() {
      static TypeDescriptor_StdVector typeDesc{ (reflection::Weak_Ptr_Wrapper<T>*) nullptr };
      return &typeDesc;
    }
  };

} // namespace reflect

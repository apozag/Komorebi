#pragma once
#include <cstddef>
#include <string>
#include <vector>

namespace reflection {

  struct TypeDescriptor;

  class TypeVisitor;

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

    template <typename T, typename std::enable_if<IsReflected<T>::value, int>::type = 0>
    static const TypeDescriptor* getDynamic(const void* pObj) {
      return &((T*)pObj)->GetReflectionDynamic();
    }

    template <typename T, typename std::enable_if<!IsReflected<T>::value, int>::type = 0>
    static const TypeDescriptor* getDynamic(const void* /* unused */) {
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
    static const TypeDescriptor* getDynamic(const void* pObj) {
      return DefaultResolver::getDynamic<T>(pObj);
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
    ~TypeDescriptor() {}
    virtual const std::string getFullName() const;
    virtual void Accept(TypeVisitor* visitor) const;
    virtual std::string GetValueStr(const void* obj) const = 0;
    virtual void SetValueFromString(void* pObj, const char* valueCStr) const = 0;
  };

  //--------------------------------------------------------
  // Type descriptor for user-defined structs/classes
  //--------------------------------------------------------

  struct TypeDescriptor_Struct : public TypeDescriptor {
    struct Member {
      const char* name;
      const TypeDescriptor* type;
      void* (*getAddress)(const void*);
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

    virtual void Accept(TypeVisitor* visitor) const override;

    int getFirstMemberIdx() const;
  private:
    std::string GetValueStr(const void* obj) const override { return {}; }
    void SetValueFromString(void* pObj, const char* valueCStr) const override {}
  };

  //--------------------------------------------------------
  // Type descriptor for pointer to struct/class
  //--------------------------------------------------------

  constexpr const char* __PtrIdAttName = "PtrId";
  constexpr const char* __PtrTypeAttName = "Type";

  struct TypeDescriptor_Ptr : public TypeDescriptor {

    TypeDescriptor_Ptr(void (*init)(TypeDescriptor_Ptr*)) : TypeDescriptor{ nullptr, 0 }
    {
      init(this);
    }
    TypeDescriptor_Ptr(const char* name, size_t size) : TypeDescriptor{ nullptr, 0 }
    {}

    const TypeDescriptor* (*getDynamicType)(const void*);
  private:
    std::string GetValueStr(const void* obj) const override { return {}; }
  };

  struct TypeDescriptor_Owned_Ptr : public TypeDescriptor_Ptr {

    TypeDescriptor_Owned_Ptr(void (*init)(TypeDescriptor_Ptr*)) : TypeDescriptor_Ptr{ init }
    {}
    TypeDescriptor_Owned_Ptr(const char* name, size_t size) : TypeDescriptor_Ptr{ nullptr, 0 }
    {}

    virtual void Accept(TypeVisitor* visitor) const override;

  private:
    std::string GetValueStr(const void* obj) const override { return {}; }
    void SetValueFromString(void* pObj, const char* valueCStr) const override {}
  };

  struct TypeDescriptor_Weak_Ptr : public TypeDescriptor_Ptr {

    TypeDescriptor_Weak_Ptr(void (*init)(TypeDescriptor_Ptr*)) : TypeDescriptor_Ptr{ init }
    {}
    TypeDescriptor_Weak_Ptr(const char* name, size_t size) : TypeDescriptor_Ptr{ nullptr, 0 }
    {}

    virtual void Accept(TypeVisitor* visitor) const override;
   
  private:
    std::string GetValueStr(const void* obj) const override { return {}; }
    void SetValueFromString(void* pObj, const char* valueCStr) const override {}
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

    virtual void Accept(TypeVisitor* visitor) const override;

    virtual const std::string getFullName() const override;
   
  private:
    std::string GetValueStr(const void* obj) const override { return {}; }
    void SetValueFromString(void* pObj, const char* valueCStr) const override {}
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

  template <typename T>
  class TypeResolver<std::vector<Owned_Ptr_Wrapper<T>>> {
  public:
    static TypeDescriptor* get() {
      static TypeDescriptor_StdVector typeDesc{ (reflection::Owned_Ptr_Wrapper<T>*) nullptr };
      return &typeDesc;
    }
  };


} // namespace reflect

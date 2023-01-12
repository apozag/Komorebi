#include "Node.h"

REFLECT_STRUCT_BASE_BEGIN(Node)
REFLECT_STRUCT_MEMBER(m_entities)
REFLECT_STRUCT_MEMBER(m_localTransform)
REFLECT_STRUCT_MEMBER(m_children)
REFLECT_STRUCT_END(Node)

//void Node::initReflection(reflection::TypeDescriptor_Struct* typeDesc) {
//  using T = Node;
//  typeDesc->name = "Node";
//  typeDesc->size = sizeof(T);
//  typeDesc->parentTypeDesc = nullptr;
//  typeDesc->construct = [](void* obj) {  new (obj) Node(); };
//  typeDesc->setup = [](void* obj) { ((Node*)obj)->SetupStatic(); };
//  typeDesc->members = {
//    {"m_entities", offsetof(T, m_entities), reflection::TypeResolver<decltype(T::m_entities)>::get()},
//    {"m_localTransform", offsetof(T, m_localTransform), reflection::TypeResolver<decltype(T::m_localTransform)>::get()},
//    {"m_children", offsetof(T, m_children), reflection::TypeResolver<decltype(T::m_children)>::get()},
//  };
//  reflection::ReflectionHelper::RegisterTypeDesc(typeDesc);
//}
//const reflection::TypeDescriptor_Struct& __typeDesc_Node = Node::GetReflection();

IMPLEMENT_REFLECTION_POINTER(Node)
#pragma once

#include "Core/Reflection/TypeDescriptors.h"

namespace reflection {

  class EditorTypeDescriptor_Struct : public TypeDescriptor_Struct {
    std::vector<Member> editorMembers;
  };

}



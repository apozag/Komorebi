#pragma once

#include "Graphics/Bindables/Bindable.h"

class StateBindable : public Bindable {
public:
	virtual void Bind() const = 0;
	virtual void Unbind() const = 0;

	REFLECT()

};

DECLARE_REFLECTION_POINTER(StateBindable)
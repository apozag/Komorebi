#pragma once

#include "Bindable.h"

class StateBindable : public Bindable {
	virtual void Bind() const = 0;
	virtual void Unbind() const = 0;
};
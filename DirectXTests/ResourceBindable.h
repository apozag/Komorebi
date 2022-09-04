#pragma once

#include "Bindable.h"

class ResourceBindable : public Bindable {
public:
	virtual void Bind() const = 0;
	virtual void Unbind() const = 0;
	virtual void Update() {};
protected:
	ResourceBindable(unsigned int slot) : m_slot(slot) {}
	ResourceBindable() : m_slot(0) {}
	unsigned int m_slot;
};

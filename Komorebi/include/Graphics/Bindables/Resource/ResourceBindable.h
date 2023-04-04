#pragma once

#include "Graphics/Bindables/Bindable.h"

namespace gfx {

	class ResourceBindable : public Bindable {
	public:
		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual void Update() {};
		unsigned int m_slot;

		REFLECT()

	protected:
		ResourceBindable(unsigned int slot) : m_slot(slot) {}
		ResourceBindable() : m_slot(0) {}
	};

}

DECLARE_REFLECTION_POINTER(gfx::ResourceBindable)
DECLARE_REFLECTION_VECTOR(gfx::ResourceBindable*)
DECLARE_REFLECTION_VECTOR(OWNED_PTR(gfx::ResourceBindable))

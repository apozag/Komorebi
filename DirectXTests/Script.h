#pragma once

#include "Entity.h"

class Script : public Entity{
	friend class ScriptDispatcher;
public:
	Script() = delete;
	void Insert(const Transform& worldTransform)override;

	virtual void Start() {}
	virtual void Update() {}

private:
	bool m_initialized = false;
};
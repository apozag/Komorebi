#pragma once

#include "Entities/Entity.h"

class Script : public Entity{
	friend class ScriptDispatcher;
public:
	void Insert(Node* node, const Transform& worldTransform)override;
	REFLECT()
private:
	virtual void Start(Node* node) = 0;
	virtual void Update(Node* node) = 0;

	bool m_started = false;
};
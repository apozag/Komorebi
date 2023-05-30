#include "ChangeColor.h"
#include "Core/Engine.h"
#include "Scene/Node.h"
#include "Graphics/Material.h"
#include "Entities/Drawable.h"

void ChangeColor::Start(Node* node) {
	Drawable* drawable = node->GetEntity<Drawable>();
	m_material = drawable->m_material;
}

void ChangeColor::Update(Node* node) {
	
	m_color[0] += Engine::GetDeltaTime()*0.1f;
	m_color[1] += Engine::GetDeltaTime()*0.1f;
	m_color[2] += Engine::GetDeltaTime()*0.1f;
	
	m_material->SetVector4("color", m_color);
}
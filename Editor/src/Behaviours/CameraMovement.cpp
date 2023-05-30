#include "Behaviours/CameraMovement.h"

//#include <d3d11.h>

#include "Core/Reflection/ReflectionImplMacros.h"

#include "Core/Engine.h"
#include "Core/Window.h"
#include "Scene/Node.h"
#include "Scene/Scene.h"

void CameraMovement::Start(Node* node) {
	Transform cameraTransform = node->m_localTransform;
	const DirectX::SimpleMath::Vector3& rot = cameraTransform.GetRotationEuler();
	m_yaw = rot.x;
	m_pitch = rot.y;
}

void CameraMovement::Update(Node* node) {

	Transform& cameraTransform = node->m_localTransform;

	float dt = Engine::GetDeltaTime();

	const Window* window = GetWindow();

	static float lastX = (float)window->m_mouse.GetX();
	static float lastY = (float)window->m_mouse.GetX();
	if (window->m_mouse.IsLeftPressed()) {
		float x = window->m_mouse.GetX();
		float y = window->m_mouse.GetY();
		m_yaw += (y - lastY) * m_sensitivity * dt;
		m_pitch += (x - lastX) * m_sensitivity * dt;

		lastX = x;
		lastY = y;
	}
	lastX = window->m_mouse.GetX();
	lastY = window->m_mouse.GetY();

	float speed = m_speed;

	// Shift key
	if (window->m_keyboard.KeyIsPressed('\x10')) {
		speed *= 2;
	}

	DirectX::SimpleMath::Vector3 cameraPos = cameraTransform.GetPosition();
	if (window->m_keyboard.KeyIsPressed('W')) {
		cameraPos += cameraTransform.GetForward() * -speed * dt;
	}
	if (window->m_keyboard.KeyIsPressed('S')) {
		cameraPos += cameraTransform.GetForward() * speed * dt;
	}
	if (window->m_keyboard.KeyIsPressed('D')) {
		cameraPos += cameraTransform.GetRight() * speed * dt;
	}
	if (window->m_keyboard.KeyIsPressed('A')) {
		cameraPos += cameraTransform.GetRight() * -speed * dt;
	}
	if (window->m_keyboard.KeyIsPressed('E')) {
		cameraPos += cameraTransform.GetUp() * speed * dt;
	}
	if (window->m_keyboard.KeyIsPressed('Q')) {
		cameraPos += cameraTransform.GetUp() * -speed * dt;
	}

	cameraTransform = Transform(
		DirectX::XMMatrixMultiply(
			DirectX::XMMatrixRotationRollPitchYawFromVector({ m_yaw, m_pitch, 0.f, 0.f }),
			DirectX::XMMatrixTranslationFromVector(cameraPos)
		)
	);
}

REFLECT_STRUCT_BEGIN(CameraMovement, Script)
REFLECT_STRUCT_MEMBER(m_speed)
REFLECT_STRUCT_MEMBER(m_sensitivity)
REFLECT_STRUCT_END(CameraMovement)

REGISTER_ENTITY(CameraMovement)

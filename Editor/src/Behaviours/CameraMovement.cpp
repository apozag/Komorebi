#include "Behaviours/CameraMovement.h"

#include "Core/Reflection/ReflectionImplMacros.h"

#include "Core/Engine.h"
#include "Core/Window.h"
#include "Scene/Node.h"
#include "Scene/Scene.h"

void CameraMovement::Run(Node* node) {

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
		float rotateR = (y - lastY) * m_sensitivity * dt;
		float rotateU = (x - lastX) * m_sensitivity * dt;

		cameraTransform.RotateEulerLocal(DirectX::SimpleMath::Vector3(rotateR, rotateU, 0));

		lastX = x;
		lastY = y;
	}
	lastX = window->m_mouse.GetX();
	lastY = window->m_mouse.GetY();

	if (window->m_keyboard.KeyIsPressed('W')) {
		cameraTransform.TranslateLocal(DirectX::SimpleMath::Vector3(0, 0, 1) * m_speed * dt);
	}
	if (window->m_keyboard.KeyIsPressed('S')) {
		cameraTransform.TranslateLocal(DirectX::SimpleMath::Vector3(0, 0, 1) * -m_speed * dt);
	}
	if (window->m_keyboard.KeyIsPressed('D')) {
		cameraTransform.TranslateLocal(DirectX::SimpleMath::Vector3(1, 0, 0) * m_speed * dt);
	}
	if (window->m_keyboard.KeyIsPressed('A')) {
		cameraTransform.TranslateLocal(DirectX::SimpleMath::Vector3(1, 0, 0) * -m_speed * dt);
	}
	if (window->m_keyboard.KeyIsPressed('E')) {
		cameraTransform.TranslateLocal(DirectX::SimpleMath::Vector3(0, 1, 0) * m_speed * dt);
	}
	if (window->m_keyboard.KeyIsPressed('Q')) {
		cameraTransform.TranslateLocal(DirectX::SimpleMath::Vector3(0, 1, 0) * -m_speed * dt);
	}
}

REFLECT_STRUCT_BEGIN(CameraMovement, Script)
REFLECT_STRUCT_MEMBER(m_speed)
REFLECT_STRUCT_MEMBER(m_sensitivity)
REFLECT_STRUCT_END(CameraMovement)

REGISTER_ENTITY(CameraMovement)

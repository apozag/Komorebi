#include "CameraMovement.h"
#include "Scene/Node.h"

void CameraMovement::Run(Node* node) {

}

void CameraMovement::Update(Node* node) {

	Transform& cameraTransform = node->m_localTransform;

	float dt = Engine::GetDeltaTime();

	const Window* window = GetWindow();

	float rotateSpeed = 0.3f;
	static float lastX = window->mouse.GetX();
	static float lastY = window->mouse.GetX();
	if (window->mouse.IsLeftPressed()) {
		float x = window->mouse.GetX();
		float y = window->mouse.GetY();
		float rotateR = (y - lastY) * rotateSpeed * dt;
		float rotateU = (x - lastX) * rotateSpeed * dt;

		cameraTransform.RotateEulerLocal(DirectX::SimpleMath::Vector3(rotateR, rotateU, 0));

		lastX = x;
		lastY = y;
	}
	lastX = window->mouse.GetX();
	lastY = window->mouse.GetY();

	float moveSpeed = 30.0f;

	if (window->keyboard.KeyIsPressed('W')) {
		cameraTransform.TranslateLocal(DirectX::SimpleMath::Vector3(0, 0, 1) * moveSpeed * dt);
	}
	if (window->keyboard.KeyIsPressed('S')) {
		cameraTransform.TranslateLocal(DirectX::SimpleMath::Vector3(0, 0, 1) * -moveSpeed * dt);
	}
	if (window->keyboard.KeyIsPressed('D')) {
		cameraTransform.TranslateLocal(DirectX::SimpleMath::Vector3(1, 0, 0) * moveSpeed * dt);
	}
	if (window->keyboard.KeyIsPressed('A')) {
		cameraTransform.TranslateLocal(DirectX::SimpleMath::Vector3(1, 0, 0) * -moveSpeed * dt);
	}
	if (window->keyboard.KeyIsPressed('E')) {
		cameraTransform.TranslateLocal(DirectX::SimpleMath::Vector3(0, 1, 0) * moveSpeed * dt);
	}
	if (window->keyboard.KeyIsPressed('Q')) {
		cameraTransform.TranslateLocal(DirectX::SimpleMath::Vector3(0, 1, 0) * -moveSpeed * dt);
	}
}

REFLECT_STRUCT_BEGIN(CameraMovement, Script)
REFLECT_STRUCT_END(CameraMovement)
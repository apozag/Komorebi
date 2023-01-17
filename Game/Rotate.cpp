#include "Rotate.h"
#include "Scene/Node.h"

void Rotate::Run(Node* node) {

}

void Rotate::Update(Node * node) {
	node->m_localTransform.RotateEuler({ 0,0,3.14f * Engine::GetDeltaTime() });
}
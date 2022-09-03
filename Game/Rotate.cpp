#include "Rotate.h"
#include "Node.h"

void Rotate::Start(Node* node) {

}

void Rotate::Update(Node * node) {
	node->localTransform.RotateEuler({ 0,0,3.14f * Engine::GetDeltaTime() });
}
#pragma once
#include "Window.h"
#include "Camera.h"
#include "Renderer.h"
#include "Scene.h"

class Game {
public:
	Game();
	int Start();
	void Update(float dt);
private:
	Window* window;
	Renderer* renderer;
	Scene scene;
	float dt;
};

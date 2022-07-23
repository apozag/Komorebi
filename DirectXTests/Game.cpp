#include "Game.h"
#include "Timer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"
#include "InputLayout.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "Texture2D.h"
#include "CubeTexture.h"
#include "RenderTarget.h"
#include "Sampler.h"
#include "Rasterizer.h"
#include "Viewport.h"
#include "DepthStencilState.h"
#include "Model.h" 
#include "Pass.h"
#include "Defines.h"

#include <iostream>
#include <thread>
#include <string>
#include <vector>

Game::Game() {
	window = new Window(800, 800, "Game");
	renderer = new Renderer(*window->gfx);
	dt = 0.01f;
}

int Game::Start() {
	long long minFrameTime = 1000/60;
	Timer timer;

	window->gfx->Init();	

	Graphics& gfx = *window->gfx;

	scene.LoadScene(gfx);

	while (true) {
		
		if (const auto ecode = Window::ProcessMessages()) {
			return *ecode;
		}
		
		Update(dt);		
		scene.RenderTraverse(gfx, *renderer);

		gfx.ClearBuffer(1, 1, 1);	
		renderer->Render(gfx);
		gfx.SwapBuffers();

		long long elapsed = timer.Mark();
		long long to_sleep = minFrameTime - elapsed;
		to_sleep = to_sleep < 0 ? 0 : to_sleep;
		std::this_thread::sleep_for(std::chrono::milliseconds(to_sleep));
	}
	
}

void Game::Update(float dt) {	

	// Camera controls

	Transform& cameraTransform = scene.GetMainCameraNode()->localTransform;

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

	float moveSpeed = 10.0f;

	if (window->keyboard.KeyIsPressed('W')) {
		cameraTransform.TranslateLocal(DirectX::SimpleMath::Vector3(0,0,1) * moveSpeed * dt);
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
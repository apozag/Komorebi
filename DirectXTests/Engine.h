#pragma once

class Scene;
class Renderer;
class Window;

class Engine {
public:

	Engine() = delete;

	static int Start(const char* windowTitle, int windowWidth, int windowHeight, float targetFrameRate);

	static float GetDeltaTime() { return m_dt; }

	static Scene* m_activeScene;

private:
	static Renderer* m_renderer;
	static Window* m_window;

	static float m_dt;
};
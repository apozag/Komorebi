#pragma once

#include <queue>

class Mouse {
	friend class Window;
public:
	class Event {
	public:
		enum Type {
			LPRESS,
			LRELEASE,
			RPRESS, 
			RRELEASE,
			WHEELUP,
			WHEELDOWN,
			MOVE,
			ENTER,
			LEAVE,
			INVALID
		};
	private:
		Type type;
		int x, y;
		bool leftIsPressed;
		bool rightIsPressed;
	public:
		Event(Type type, int x, int y) 
			:
			type(type),
			x(x),
			y(y)
		{}
		Type GetType() { return type; }
		int GetX() { return x; }
		int GetY() { return y; }
	};
public:
	bool IsRightPressed() const noexcept { return rightPressed; }
	bool IsLeftPressed() const noexcept { return leftPressed; }
	float GetX()const noexcept { return x; }
	float GetY()const noexcept { return y; }
	Event Read() noexcept;
	bool IsEmpty() const noexcept { return buffer.empty(); }
	void Flush() noexcept;
private:
	void OnMouseMove(int x, int y) noexcept;
	void OnLeftPressed(int x, int y) noexcept;
	void OnRightPressed(int x, int y) noexcept;
	void OnLeftReleased(int x, int y) noexcept;
	void OnRightReleased(int x, int y) noexcept;
	void OnWheelUp(int x, int y) noexcept;
	void OnWheelDown(int x, int y) noexcept;
	void OnEnter(int x, int y) noexcept;
	void OnLeave(int x, int y) noexcept;
	void TrimBuffer() noexcept;
private:
	bool rightPressed = false;
	bool leftPressed = false;
	bool mouseOnWindow = false;
	std::queue<Event> buffer;
	unsigned int bufferSize = 16u;
	int x, y;
};

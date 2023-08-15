#include "Input/Mouse.h"

Mouse::Event Mouse::Read() noexcept {
	Event e = buffer.front();
	buffer.pop();
	return e;
}

void Mouse::Flush() noexcept {
	buffer = std::queue<Event>();
}

void Mouse::OnMouseMove(int x, int y) noexcept {
	buffer.push(Event(Event::Type::MOVE, x, y));
	this->x = x;
	this->y = y;
	TrimBuffer();
}

void Mouse::OnLeftPressed(int x, int y) noexcept {
	buffer.push(Event(Event::Type::LPRESS, x, y));
	this->leftPressed = true;
	this->x = x;
	this->y = y;
	TrimBuffer();
}

void Mouse::OnLeftReleased(int x, int y) noexcept {
	buffer.push(Event(Event::Type::LRELEASE, x, y));
	this->leftPressed = false;
	this->x = x;
	this->y = y;
	TrimBuffer();
}

void Mouse::OnRightPressed(int x, int y) noexcept {
	buffer.push(Event(Event::Type::RPRESS, x, y));
	this->rightPressed = true;
	this->x = x;
	this->y = y;
	TrimBuffer();
}

void Mouse::OnRightReleased(int x, int y) noexcept {
	buffer.push(Event(Event::Type::RRELEASE, x, y));
	this->rightPressed = false;
	this->x = x;
	this->y = y;
	TrimBuffer();
}

void Mouse::OnWheelDown(int x, int y) noexcept {
	buffer.push(Event(Event::Type::WHEELDOWN, x, y));
	TrimBuffer();
}

void Mouse::OnWheelUp(int x, int y) noexcept {
	buffer.push(Event(Event::Type::WHEELUP, x, y));
	TrimBuffer();
}

void Mouse::OnLeave(int x, int y) noexcept {
	mouseOnWindow = false;
	buffer.push(Event(Event::Type::LEAVE, x, y));
	TrimBuffer();
}

void Mouse::OnEnter(int x, int y) noexcept {
	mouseOnWindow = true;
	TrimBuffer();
}

void Mouse::TrimBuffer() noexcept {
	while (buffer.size() > bufferSize) {
		buffer.pop();
	}
}
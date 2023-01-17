
#include "Input/Keyboard.h"


bool Keyboard::KeyIsPressed(unsigned char keycode) const noexcept{
	return keystates[keycode];
}

Keyboard::Event Keyboard::ReadKey() noexcept {
	if (keybuffer.size() > 0u) {
		Event e = keybuffer.front();
		keybuffer.pop();
		return e;
	}
	return Event(Keyboard::Event::Type::INVALID, 0);
}

bool Keyboard::KeyIsEmpty() const noexcept{
	return keybuffer.size() == 0;
}

void Keyboard::FlushKey() noexcept {
	keybuffer = std::queue<Event>();
}

char Keyboard::ReadChar() noexcept {
	if (charbuffer.size() > 0) {
		char c = charbuffer.front();
		charbuffer.pop();
		return c;
	}
	return 0;
}

bool Keyboard::CharIsEmpty() const noexcept{
	return charbuffer.size() == 0;
}

void Keyboard::FlushChar() noexcept {
	charbuffer = std::queue<char>();
}

void Keyboard::Flush() noexcept {
	FlushKey();
	FlushChar();
}

void Keyboard::EnableAutoRepeat() noexcept{
	autorepeatEnabled = true;
}

void Keyboard::DisableAutoRepeat() noexcept{
	autorepeatEnabled = false;
}

bool Keyboard::AutorepeatIsEnabled() const noexcept {
	return autorepeatEnabled;
}

void Keyboard::OnKeyPressed(unsigned char keycode) noexcept {
	keystates[keycode] = true; 
	keybuffer.push(Keyboard::Event(Keyboard::Event::Type::PRESS, keycode));
	TrimBuffer(keybuffer);
}

void Keyboard::OnKeyReleased(unsigned char keycode) noexcept {
	keystates[keycode] = false;
	keybuffer.push(Keyboard::Event(Keyboard::Event::Type::RELEASE, keycode));
	TrimBuffer(keybuffer);
}

void Keyboard::OnChar(char character) noexcept {
	charbuffer.push(character);
	TrimBuffer(charbuffer);
}

void Keyboard::ClearState() noexcept {
	keystates.reset();
}

template<typename T>
void Keyboard::TrimBuffer(std::queue<T>& buffer) noexcept {
	while (buffer.size() > bufferSize) {
		buffer.pop();
	}
}
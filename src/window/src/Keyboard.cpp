#include "window/Keyboard.h"
#include <cassert>

using namespace wnd;

[[nodiscard]]
bool
Keyboard::IsKeyPressed(uint32_t keycode) const noexcept
{
	return keyStates[keycode];
}

[[nodiscard]]
std::optional<Keyboard::KeyEvent>
Keyboard::ReadKey() noexcept
{
	if (keyBuffer.empty())
		return std::nullopt;
	auto e = keyBuffer.front();
	keyBuffer.pop();
	return e;
}

[[nodiscard]]
std::optional<Keyboard::CharEvent>
Keyboard::ReadChar() noexcept
{
	if (charBuffer.empty())
		return std::nullopt;
	auto e = charBuffer.front();
	charBuffer.pop();
	return e;
}

[[nodiscard]]
bool
Keyboard::Empty() const noexcept
{
	return keyBuffer.empty();
}

void
Keyboard::Clear() noexcept
{
	keyBuffer.clear();
}

void
Keyboard::OnKeyDown(uint32_t keycode) noexcept
{
	assert(keycode < nKeys);
	keyStates[keycode] = true;
	keyBuffer.emplace(KeyEvent::Type::kDown, keycode);
}

void
Keyboard::OnKeyUp(uint32_t keycode) noexcept
{
	assert(keycode < nKeys);
	keyStates[keycode] = false;
	keyBuffer.emplace(KeyEvent::Type::kUp, keycode);
}

void
Keyboard::OnChar(char32_t ch) noexcept
{
	charBuffer.emplace(ch);
}

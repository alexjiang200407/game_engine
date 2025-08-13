#include "Keyboard.h"
#include <cassert>

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
	keyBuffer = {};
}

void
Keyboard::OnKeyDown(uint32_t keycode) noexcept
{
	assert(keycode < nKeys);
	keyStates[keycode] = true;
	keyBuffer.push({ KeyEvent::Type::kDown, keycode });
}

void
Keyboard::OnKeyUp(uint32_t keycode) noexcept
{
	assert(keycode < nKeys);
	keyStates[keycode] = false;
	keyBuffer.push({ KeyEvent::Type::kUp, keycode });
}

void
Keyboard::OnChar(char32_t ch) noexcept
{
	charBuffer.push({ ch });
}

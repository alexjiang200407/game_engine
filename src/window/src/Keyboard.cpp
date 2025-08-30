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
std::optional<KeyEvent>
Keyboard::ReadKey() noexcept
{
	if (keyBuffer.empty())
		return std::nullopt;
	auto e = std::move(keyBuffer.front());
	keyBuffer.pop();
	return e;
}

[[nodiscard]]
std::optional<CharEvent>
Keyboard::ReadChar() noexcept
{
	if (charBuffer.empty())
		return std::nullopt;
	auto e = std::move(charBuffer.front());
	charBuffer.pop();
	return e;
}

void
Keyboard::Clear() noexcept
{
	keyStates.reset();
	keyBuffer.clear();
	charBuffer.clear();
}

void
wnd::Keyboard::DispatchInputEvents()
{
	while (auto keyEvt = ReadKey())
	{
		Produce(*keyEvt);
	}

	for (uint32_t i = 0; i < nKeys; ++i)
	{
		if (keyStates.test(i))
		{
			Produce(KeyEvent(KeyEvent::Type::kPressed, i));
		}
	}
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

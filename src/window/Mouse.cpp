#include "window/Mouse.h"

using namespace wnd;

[[nodiscard]] bool
Mouse::Empty() const noexcept
{
	return mouseBuffer.empty();
}

void
Mouse::Clear() noexcept
{
	mouseBuffer.clear();
}

[[nodiscard]]
std::pair<int, int>
Mouse::GetPoint() const noexcept
{
	return std::make_pair(x, y);
}

std::optional<Mouse::Event>
Mouse::ReadEvent() noexcept
{
	if (Empty())
		return std::nullopt;

	auto ret = mouseBuffer.front();
	mouseBuffer.pop();
	return ret;
}

bool
Mouse::HasStateFlag(StateFlags flag) const noexcept
{
	return state.all(flag);
}

void
Mouse::SetPos(int newX, int newY) noexcept
{
	x = newX;
	y = newY;
}

void
Mouse::OnMouseMove(int dx, int dy) noexcept
{
	x += dx;
	y += dy;
	mouseBuffer.emplace(Event::Type::kMove, state, x, y);
}

void
Mouse::OnLeftDown() noexcept
{
	state.set(StateFlags::kLeftDown);
	mouseBuffer.emplace(Event::Type::kLPress, state, x, y);
}

void
Mouse::OnRightDown() noexcept
{
	state.set(StateFlags::kRightDown);
	mouseBuffer.emplace(Event::Type::kRPress, state, x, y);
}

void
Mouse::OnMiddleDown() noexcept
{
	state.set(StateFlags::kMiddleDown);
	mouseBuffer.emplace(Event::Type::kMPress, state, x, y);
}

void
Mouse::OnLeftUp() noexcept
{
	state.reset(StateFlags::kLeftDown);
	mouseBuffer.emplace(Event::Type::kLRelease, state, x, y);
}

void
Mouse::OnRightUp() noexcept
{
	state.reset(StateFlags::kRightDown);
	mouseBuffer.emplace(Event::Type::kRRelease, state, x, y);
}

void
Mouse::OnMiddleUp() noexcept
{
	state.reset(StateFlags::kMiddleDown);
	mouseBuffer.emplace(Event::Type::kMRelease, state, x, y);
}

void
Mouse::OnWheel(int wheelDelta) noexcept
{
	mouseBuffer.emplace(Event::Type::kWheel, state, x, y, wheelDelta);
}

void
Mouse::OnMouseLeave() noexcept
{
	state.reset(StateFlags::kInsideWindow);
	mouseBuffer.emplace(Event::Type::kLeave, state, x, y);
}

void
Mouse::OnMouseEnter() noexcept
{
	state.set(StateFlags::kInsideWindow);
	mouseBuffer.emplace(Event::Type::kEnter, state, x, y);
}

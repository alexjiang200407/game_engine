#include "window/Mouse.h"

[[nodiscard]] bool
Mouse::Empty() const noexcept
{
	return mouseEvents.empty();
}

void
Mouse::Clear()
{
	mouseEvents = {};
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

	auto ret = mouseEvents.front();
	mouseEvents.pop();
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
Mouse::OnMouseMove(int dx, int dy)
{
	x += dx;
	y += dy;
	mouseEvents.emplace(Event::Type::kMove, state, x, y);
}

void
Mouse::OnLeftDown()
{
	state.set(StateFlags::kLeftDown);
	mouseEvents.emplace(Event::Type::kLPress, state, x, y);
}

void
Mouse::OnRightDown()
{
	state.set(StateFlags::kRightDown);
	mouseEvents.emplace(Event::Type::kRPress, state, x, y);
}

void
Mouse::OnMiddleDown()
{
	state.set(StateFlags::kMiddleDown);
	mouseEvents.emplace(Event::Type::kMPress, state, x, y);
}

void
Mouse::OnLeftUp()
{
	state.reset(StateFlags::kLeftDown);
	mouseEvents.emplace(Event::Type::kLRelease, state, x, y);
}

void
Mouse::OnRightUp()
{
	state.reset(StateFlags::kRightDown);
	mouseEvents.emplace(Event::Type::kRRelease, state, x, y);
}

void
Mouse::OnMiddleUp()
{
	state.reset(StateFlags::kMiddleDown);
	mouseEvents.emplace(Event::Type::kMRelease, state, x, y);
}

void
Mouse::OnWheel(int wheelDelta)
{
	mouseEvents.emplace(Event::Type::kWheel, state, x, y, wheelDelta);
}

void
Mouse::OnMouseLeave()
{
	state.reset(StateFlags::kInsideWindow);
	mouseEvents.emplace(Event::Type::kLeave, state, x, y);
}

void
Mouse::OnMouseEnter()
{
	state.set(StateFlags::kInsideWindow);
	mouseEvents.emplace(Event::Type::kEnter, state, x, y);
}

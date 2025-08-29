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

std::optional<MouseEvent>
Mouse::ReadEvent() noexcept
{
	if (Empty())
		return std::nullopt;

	auto ret = std::move(mouseBuffer.front());
	mouseBuffer.pop();
	return ret;
}

bool
Mouse::HasStateFlag(MouseEvent::StateFlags flag) const noexcept
{
	return state.all(flag);
}

int
wnd::Mouse::GetWheelOffset() const noexcept
{
	return wheelOffset;
}

void
Mouse::OnMouseMove(int dx, int dy) noexcept
{
	x += dx;
	y += dy;
	mouseBuffer.emplace(MouseEvent::Type::kMove, state, x, y, 0, dx, dy);
}

void
Mouse::OnLeftDown() noexcept
{
	state.set(MouseEvent::StateFlags::kLeftDown);
	mouseBuffer.emplace(MouseEvent::Type::kLPress, state, x, y);
}

void
Mouse::OnRightDown() noexcept
{
	state.set(MouseEvent::StateFlags::kRightDown);
	mouseBuffer.emplace(MouseEvent::Type::kRPress, state, x, y);
}

void
Mouse::OnMiddleDown() noexcept
{
	state.set(MouseEvent::StateFlags::kMiddleDown);
	mouseBuffer.emplace(MouseEvent::Type::kMPress, state, x, y);
}

void
Mouse::OnLeftUp() noexcept
{
	state.reset(MouseEvent::StateFlags::kLeftDown);
	mouseBuffer.emplace(MouseEvent::Type::kLRelease, state, x, y);
}

void
Mouse::OnRightUp() noexcept
{
	state.reset(MouseEvent::StateFlags::kRightDown);
	mouseBuffer.emplace(MouseEvent::Type::kRRelease, state, x, y);
}

void
Mouse::OnMiddleUp() noexcept
{
	state.reset(MouseEvent::StateFlags::kMiddleDown);
	mouseBuffer.emplace(MouseEvent::Type::kMRelease, state, x, y);
}

void
Mouse::OnWheel(int wheelDelta) noexcept
{
	mouseBuffer.emplace(MouseEvent::Type::kWheel, state, x, y, wheelDelta);
	util::math::safe_add_assign(wheelOffset, wheelDelta);
}

void
Mouse::OnMouseLeave() noexcept
{
	state.reset(MouseEvent::StateFlags::kInsideWindow);
	mouseBuffer.emplace(MouseEvent::Type::kLeave, state, x, y);
}

void
Mouse::OnMouseEnter() noexcept
{
	state.set(MouseEvent::StateFlags::kInsideWindow);
	mouseBuffer.emplace(MouseEvent::Type::kEnter, state, x, y);
}

void
wnd::Mouse::DispatchInputEvents()
{
	while (auto mouseEvt = ReadEvent())
	{
		Produce(*mouseEvt);
	}
}

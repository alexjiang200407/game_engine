#pragma once
#include "util/RingQueue.h"
#include "util/events/Producer.h"

namespace wnd
{
	class Window;

	struct MouseEvent
	{
		enum class StateFlags
		{
			kLeftDown     = 1 << 0,
			kRightDown    = 1 << 1,
			kMiddleDown   = 1 << 2,
			kInsideWindow = 1 << 3,
		};

		using State = util::EnumSet<StateFlags, uint8_t>;

		enum class Type
		{
			kLPress,
			kLRelease,
			kRPress,
			kRRelease,
			kMPress,
			kMRelease,
			kWheel,
			kMove,
			kEnter,
			kLeave,
			kInvalid,
		} type;

		State state;
		int   x, y;
		int   wheelDelta = 0;
		int   dx = 0, dy = 0;
	};

	class Mouse : public util::Producer<MouseEvent>
	{
		friend class Window;

	public:
		[[nodiscard]] bool
		Empty() const noexcept;

		void
		Clear() noexcept;

		[[nodiscard]] std::pair<int, int>
		GetPoint() const noexcept;

		[[nodiscard]] std::optional<MouseEvent>
		ReadEvent() noexcept;

		[[nodiscard]] bool
		HasStateFlag(MouseEvent::StateFlags flag) const noexcept;

		[[nodiscard]] int
		GetWheelOffset() const noexcept;

		void
		DispatchInputEvents();

	private:
		Mouse() noexcept = default;

		void
		OnMouseMove(int dx, int dy) noexcept;

		void
		OnLeftDown() noexcept;

		void
		OnRightDown() noexcept;

		void
		OnMiddleDown() noexcept;

		void
		OnLeftUp() noexcept;

		void
		OnRightUp() noexcept;

		void
		OnMiddleUp() noexcept;

		void
		OnWheel(int wheelDelta) noexcept;

		void
		OnMouseLeave() noexcept;

		void
		OnMouseEnter() noexcept;

	private:
		static constexpr auto                       mouseBufferLen = 256u;
		util::RingQueue<MouseEvent, mouseBufferLen> mouseBuffer{};
		MouseEvent::State                           state{};
		int                                         wheelOffset = 0;
		int                                         x = 0, y = 0;
	};
}

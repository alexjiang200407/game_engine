#pragma once
#include <util/RingQueue.h>

namespace wnd
{
	class Window;

	class Mouse
	{
		friend class Window;

	public:
		enum class StateFlags
		{
			kLeftDown     = 1 << 0,
			kRightDown    = 1 << 1,
			kMiddleDown   = 1 << 2,
			kInsideWindow = 1 << 3,
		};

		using State = util::EnumSet<StateFlags, uint8_t>;

		struct Event
		{
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
		};

	public:
		[[nodiscard]] bool
		Empty() const noexcept;

		void
		Clear() noexcept;

		[[nodiscard]] std::pair<int, int>
		GetPoint() const noexcept;

		[[nodiscard]] std::optional<Event>
		ReadEvent() noexcept;

		[[nodiscard]] bool
		HasStateFlag(StateFlags flag) const noexcept;

		[[nodiscard]] int
		GetWheelOffset() const noexcept;

	private:
		Mouse() noexcept = default;

		void
		SetPos(int newX, int newY) noexcept;

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
		static constexpr auto                  mouseBufferLen = 256u;
		util::RingQueue<Event, mouseBufferLen> mouseBuffer{};
		State                                  state{};
		int                                    wheelOffset = 0;
		int                                    x = 0, y = 0;
	};
}

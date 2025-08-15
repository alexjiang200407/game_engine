#pragma once

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
			int   wheelDelta;
		};

	public:
		[[nodiscard]] bool
		Empty() const noexcept;

		void
		Clear();

		[[nodiscard]] std::pair<int, int>
		GetPoint() const noexcept;

		[[nodiscard]] std::optional<Event>
		ReadEvent() noexcept;

		[[nodiscard]] bool
		HasStateFlag(StateFlags flag) const noexcept;

	private:
		Mouse() noexcept = default;

		void
		SetPos(int newX, int newY) noexcept;

		void
		OnMouseMove(int dx, int dy);

		void
		OnLeftDown();

		void
		OnRightDown();

		void
		OnMiddleDown();

		void
		OnLeftUp();

		void
		OnRightUp();

		void
		OnMiddleUp();

		void
		OnWheel(int wheelDelta);

		void
		OnMouseLeave();

		void
		OnMouseEnter();

	private:
		std::queue<Event> mouseEvents{};
		State             state{};
		int               x = 0, y = 0;
	};
}

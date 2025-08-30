#pragma once
#include "util/RingQueue.h"
#include <util/events/Producer.h>

namespace wnd
{
	class Window;

	struct KeyEvent
	{
		enum class Type
		{
			kUp,
			kPressed,
			kDown,
			kInvalid,
		} type;
		uint32_t code;
	};

	struct CharEvent
	{
		char32_t ch;
	};

	class Keyboard : public util::Producer<KeyEvent>
	{
		friend class Window;

	public:
		[[nodiscard]] bool
		IsKeyPressed(uint32_t keycode) const noexcept;

		void
		Clear() noexcept;

		void
		DispatchInputEvents();

	private:
		Keyboard() = default;

		[[nodiscard]] std::optional<KeyEvent>
		ReadKey() noexcept;

		[[nodiscard]] std::optional<CharEvent>
		ReadChar() noexcept;

		void
		OnKeyDown(uint32_t keycode) noexcept;

		void
		OnKeyUp(uint32_t keycode) noexcept;

		void
		OnChar(char32_t character) noexcept;

	private:
		static constexpr size_t               nKeys     = 256u;
		static constexpr auto                 bufferLen = 256u;
		std::bitset<nKeys>                    keyStates;
		util::RingQueue<KeyEvent, bufferLen>  keyBuffer;
		util::RingQueue<CharEvent, bufferLen> charBuffer;
	};
}

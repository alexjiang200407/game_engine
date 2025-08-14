#pragma once

class Window;

class Keyboard
{
	friend class Window;

private:
	struct KeyEvent
	{
		enum class Type
		{
			kUp,
			kDown,
			kInvalid,
		} type;
		uint32_t code;
	};

	struct CharEvent
	{
		char32_t ch;
	};

public:
	[[nodiscard]] bool
	IsKeyPressed(uint32_t keycode) const noexcept;

	[[nodiscard]] std::optional<KeyEvent>
	ReadKey() noexcept;

	[[nodiscard]] std::optional<CharEvent>
	ReadChar() noexcept;

	[[nodiscard]] bool
	Empty() const noexcept;

	void
	Clear();

private:
	Keyboard() = default;

	void
	OnKeyDown(uint32_t keycode);

	void
	OnKeyUp(uint32_t keycode);

	void
	OnChar(char32_t character);

private:
	static constexpr size_t nKeys = 256u;
	std::bitset<nKeys>      keyStates;
	std::queue<KeyEvent>    keyBuffer;
	std::queue<CharEvent>   charBuffer;
};

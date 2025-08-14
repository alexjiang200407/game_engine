#pragma once

#ifdef _WIN32

#	include "window/Window.h"

#endif  // _WIN32

class Game
{
public:
	Game() noexcept = default;

	void
	Play(Window& wnd);

private:
	void
	DoFrame();
};

#pragma once

#include "gfx/Graphics.h"
#include "util/Timer.h"
#include "window/Window.h"

class Game
{
public:
	Game();

	void
	Play();

private:
	void
	DoFrame();

private:
	util::Timer   timer;
	wnd::Window   wnd;
	gfx::Graphics gfx;
};

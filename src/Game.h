#pragma once

#include <gfx/IDrawable.h>
#include <gfx/IGraphics.h>
#include <util/Timer.h>
#include <window/Window.h>

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
	std::vector<std::unique_ptr<class gfx::IDrawable>> drawables;
	static constexpr size_t                            nDrawables = 180;

	util::Timer                     timer;
	wnd::Window                     wnd;
	std::unique_ptr<gfx::IGraphics> gfx;
};

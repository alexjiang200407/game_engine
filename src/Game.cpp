#include "Game.h"

Game::Game() : wnd(1280u, 720u), gfx(1280u, 720u) {}

void
Game::Play()
{
	while (wnd.Process())
	{
		DoFrame();
	}
}

void
Game::DoFrame()
{
	const float c = sin(timer.Peek()) / 2.0f + 0.5f;
	gfx.ClearBuffer(c, c, 1.0f);

	auto [mouseX, mouseY] = wnd.mouse.GetPoint();
	auto wheelZ           = wnd.mouse.GetWheelOffset();

	gfx.DrawTestTriangle(c, mouseX, mouseY, wheelZ);
	gfx.DrawTestTriangle(c, 0, 0, 0);
	gfx.EndFrame();
}

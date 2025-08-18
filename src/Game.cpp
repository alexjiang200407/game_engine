#include "Game.h"

Game::Game() : wnd(1280, 720), gfx(1280, 720) {}

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
	gfx.DrawTestTriangle();
	gfx.EndFrame();
}

#include "Game.h"

void
Game::Play(wnd::Window& wnd)
{
	while (wnd.Process())
	{
		DoFrame();
	}
}

void
Game::DoFrame()
{}

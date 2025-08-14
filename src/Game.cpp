#include "Game.h"

void
Game::Play(Window& wnd)
{
	while (wnd.Process())
	{
		DoFrame();
	}
}

void
Game::DoFrame()
{}

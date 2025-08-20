#include "Game.h"
#include "gfx/draw/Box.h"

Game::Game() : wnd(1280u, 720u), gfx(1280u, 720u)
{
	gfx.SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 40.0f));
}

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
	auto dt = timer.Mark();
	gfx.ClearBuffer(0.07f, 0.0f, 0.12f);

	static std::uniform_real_distribution<float> adist(0.0f, 3.1415f * 2.0f);
	static std::uniform_real_distribution<float> ddist(0.0f, 3.1415f * 2.0f);
	static std::uniform_real_distribution<float> odist(0.0f, 3.1415f * 0.3f);
	static std::uniform_real_distribution<float> rdist(6.0f, 20.0f);

	static std::mt19937 rng(std::random_device{}());
	static gfx::Box     b{ gfx, rng, adist, ddist, odist, rdist };

	b.Update(dt);
	b.Draw(gfx);

	gfx.EndFrame();
}

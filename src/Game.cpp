#include "Game.h"
#include "gfx/DrawableFactory.h"

Game::Game() : wnd(1280u, 720u), gfx(gfx::IGraphics::Make(1280u, 1080u))
{
	drawables.reserve(nDrawables);
	std::generate_n(std::back_inserter(drawables), nDrawables, gfx::DrawableFactory{ *gfx });
	gfx->SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 40.0f));
}

void
Game::Play()
{
	while (wnd.Process())
	{
		while (auto key = wnd.kbd.ReadKey())
		{
			if (key->type == wnd::Keyboard::KeyEvent::Type::kDown)
			{
				drawables.clear();
			}
		}

		DoFrame();
	}
}

void
Game::DoFrame()
{
	const auto dt = timer.Mark();
	gfx->ClearBuffer(0.07f, 0.0f, 0.12f);
	for (auto& d : drawables)
	{
		d->Update(dt);
		d->Draw(*gfx);
	}
	gfx->EndFrame();
}

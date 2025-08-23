#include "Game.h"
#include <gfx/DrawableFactory.h>

Game::Game() : wnd(1440, 900), gfx(gfx::IGraphics::Make(1440, 900))
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
	gfx->StartFrame();

	ImGui::NewFrame();

	static float speed = 1.0f;

	if (ImGui::Begin("Debug"))
	{
		ImGui::DragFloat("speed factor", &speed, 1.0f, 0.1f, 10.0f);
		ImGui::Text(
			"framerate %.3f ms/frame (%.1f FPS)",
			1000.0f / ImGui::GetIO().Framerate,
			ImGui::GetIO().Framerate);
	}
	ImGui::End();

	const auto dt = timer.Mark() * speed;
	gfx->ClearBuffer(0.07f, 0.0f, 0.12f);
	for (auto& d : drawables)
	{
		d->Update(dt);
		d->Draw(*gfx);
	}
	//ImGui::ShowDemoWindow();

	gfx->EndFrame();
}

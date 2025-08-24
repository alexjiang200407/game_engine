#include "Game.h"

Game::Game() :
	wnd(1440, 900), gfx(gfx::IGraphics::Make(1440, 900)),
	light(std::move(factory.CreatePointLight(*gfx)))
{
	assert(gfx);
	drawables.reserve(nDrawables);

	std::generate_n(std::back_inserter(drawables), nDrawables, [&]() {
		return factory.CreateBox(*gfx);
	});

	gfx->SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 40.0f));
	gfx->SetCamera(DirectX::XMMatrixTranslation(0.0f, 0.0f, 20.0f));
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
	gfx->StartFrame();

	ImGui::NewFrame();

	light->Bind(*gfx);

	static float speed = 1.0f;

	if (ImGui::Begin("Debug Window"))
	{
		ImGui::DragFloat("speed factor", &speed, 1.0f, 0.1f, 10.0f);
		ImGui::Text(
			"framerate %.3f ms/frame (%.1f FPS)",
			1000.0f / ImGui::GetIO().Framerate,
			ImGui::GetIO().Framerate);

		static char buf[256]{};
		ImGui::InputText("Text", buf, sizeof(buf));
	}
	ImGui::End();

	light->DrawControlWindow();

	if (camera.DrawControlWindow())
	{
		gfx->SetCamera(camera.GetMatrix());
	}

	const auto dt = timer.Mark() * speed;
	gfx->ClearBuffer(0.0f, 0.0f, 0.0f);
	for (auto& d : drawables)
	{
		d->Update(dt);
		d->Draw(*gfx);
	}
	light->Draw(*gfx);
	//ImGui::ShowDemoWindow();

	gfx->EndFrame();
}

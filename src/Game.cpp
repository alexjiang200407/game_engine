#include "Game.h"

Game::Game() :
	wnd(1440, 900), gfx(1440, 900), light(std::move(factory.CreatePointLight(gfx))),
	pModel(std::move(factory.CreateModel(gfx, "assets/meshes/stylized_face.glb")))
{
	gfx.SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 40.0f));
	gfx.SetCamera(DirectX::XMMatrixTranslation(0.0f, 0.0f, 20.0f));
}

void
Game::Play()
{
	while (wnd.Process())
	{
		while (auto key = wnd.kbd.ReadKey())
		{
			using Type = wnd::Keyboard::KeyEvent::Type;

			if (key->type == Type::kDown && key->code == 192)
			{
				drawDebugUI = !drawDebugUI;
			}
		}
		DoFrame();
	}
}

void
Game::DoFrame()
{
	gfx.StartFrame();

	ImGui::NewFrame();

	light->Bind(gfx, camera.GetMatrix());

	if (drawDebugUI)
	{
		if (ImGui::Begin("Framerate"))
		{
			ImGui::Text(
				"framerate %.3f ms/frame (%.1f FPS)",
				1000.0f / ImGui::GetIO().Framerate,
				ImGui::GetIO().Framerate);
		}
		ImGui::End();
		light->DrawControlWindow();

		if (camera.DrawControlWindow())
			gfx.SetCamera(camera.GetMatrix());
		pModel->DrawControlPanel();
	}

	gfx.ClearBuffer(0.0f, 0.0f, 0.0f);

	pModel->Draw(gfx);

	light->Draw(gfx);

	gfx.EndFrame();
}
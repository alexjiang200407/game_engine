#include "Game.h"

Game::Game() :
	light(std::move(factory.CreatePointLight(gfx))),
	pModel(std::move(factory.CreateModel(gfx, "assets/meshes/stylized_face.glb")))
{
	gfx.SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 40.0f));
	gfx.SetCamera(DirectX::XMMatrixTranslation(0.0f, 0.0f, 20.0f));
	camera = wnd.kbd.RegisterConsumer<scene::Camera>();
	wnd.mouse.RegisterConsumer(camera);

	ImGui::CommandRegister::Instance().Register(
		"ResizeBuffers",
		[this](unsigned int width, unsigned int height) {
			gfx.ResizeBuffers(width, height);
			wnd.ResizeWindow(width, height);
		});
}

void
Game::Play()
{
	while (wnd.Process())
	{
		if (ImGui::IsKeyPressed(ImGuiKey_GraveAccent, false))
		{
			imgui.ToggleVisibility();
		}

		if (!imgui.IsShowing())
		{
			wnd.kbd.DispatchInputEvents();
			wnd.mouse.DispatchInputEvents();
		}
		else
		{
			wnd.kbd.Clear();
			wnd.mouse.Clear();
		}

		DoFrame();
		ImGui::CommandRegister::Instance().Process();

	}
}

void
Game::DoFrame()
{
	gfx.StartFrame();

	ImGui::NewFrame();

	gfx.ClearBuffer(0.0f, 0.0f, 0.0f);

	gfx.SetCamera(camera->GetMatrix());

	light->Bind(gfx, camera->GetMatrix());

	if (imgui.IsShowing())
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

		pModel->DrawControlPanel();
		ImGui::CommandRegister::Instance().DrawControlWindow();
	}
	pModel->Draw(gfx);

	light->Draw(gfx);

	gfx.EndFrame();
}
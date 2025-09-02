#include "ImGuiManager.h"

ImGuiManager::ImGuiManager() noexcept
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	show                           = util::Settings::Module("Debug UI").Get("bInitialShown", false);
	ImGui::GetIO().MouseDrawCursor = show;
	ImGui::GetIO().FontGlobalScale = 1.3f;
}

ImGuiManager::~ImGuiManager() noexcept { ImGui::DestroyContext(); }

bool
ImGuiManager::IsShowing() const noexcept
{
	return show;
}

void
ImGuiManager::ToggleVisibility() noexcept
{
	show                           = !show;
	ImGui::GetIO().MouseDrawCursor = show;
}

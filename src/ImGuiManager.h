#pragma once

class ImGuiManager
{
public:
	ImGuiManager() noexcept;
	~ImGuiManager() noexcept;

	[[nodiscard]] bool
	IsShowing() const noexcept;

	void
	ToggleVisibility() noexcept;

private:
	bool show;
};

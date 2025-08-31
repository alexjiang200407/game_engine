#pragma once
#include <string>

namespace ImGui
{
	class CommandLine
	{
	public:
		CommandLine() = default;

		void
		DrawControlWindow() noexcept;

		void
		ProcessPending() noexcept;

	private:
		std::string                               buf;
		std::vector<std::pair<std::string, bool>> history;
		int                                       historyIdx = -1;
	};
}

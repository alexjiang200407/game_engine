#pragma once
#include <string>
#include <util/CommandContext.h>

namespace ImGui
{
	class CommandLine : public util::CommandContext<CommandLine>
	{
	public:
		CommandLine();

		void
		DrawControlWindow() noexcept;

		void
		ProcessPending() noexcept;

	private:
		static void
		Clear(CommandLine& cl) noexcept;

	private:
		std::string                               buf;
		std::vector<std::pair<std::string, bool>> displayHistory;
		std::vector<std::string>                  inputHistory;
		int                                       historyIdx = -1;
	};
}

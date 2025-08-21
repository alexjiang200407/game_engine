#include "util/logger.h"
#include <filesystem>
#include <spdlog/sinks/basic_file_sink.h>

namespace fs = std::filesystem;

using namespace std::string_literals;

void
logger::Init(const std::wstring& ws)
{
	fs::path logPath = fs::path(ws).parent_path() / "logs.txt";

	auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logPath.string(), true);

	auto log = std::make_shared<spdlog::logger>("global log", std::move(sink));

	auto logLevel = spdlog::level::trace;

	log->set_level(logLevel);
	log->flush_on(logLevel);

	spdlog::set_default_logger(std::move(log));
	spdlog::set_pattern("[%H:%M:%S:%e] [thread %t] [%l] %v"s);
}

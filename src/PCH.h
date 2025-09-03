#pragma once

#ifdef _WIN32
#	define UNICODE
#	define _UNICODE
#	define WIN32_LEAN_AND_MEAN
#	define NOMINMAX
#	include <Windows.h>
#	include <wrl/client.h>
#endif

#define __REL_FILE__ (&((__FILE__)[sizeof(PROJECT_SOURCE_DIR)]))

#include <algorithm>
#include <bitset>
#include <cassert>
#include <filesystem>
#include <format>
#include <iomanip>
#include <limits>
#include <memory>
#include <mutex>
#include <optional>
#include <queue>
#include <random>
#include <span>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

#include <util/EnumSet.h>
#include <util/Settings.h>
#include <util/logger.h>
#include <util/math.h>
#include <util/str.h>

#include <DirectXMath.h>

#include <imgui.h>

using namespace std::string_literals;
using namespace std::literals;

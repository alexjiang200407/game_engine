#pragma once

#ifdef _WIN32
#	define UNICODE
#	define _UNICODE
#	define WIN32_LEAN_AND_MEAN
#	include <Windows.h>
#	include <wrl/client.h>
#endif

#define __REL_FILE__ (&((__FILE__)[sizeof(PROJECT_SOURCE_DIR)]))

#include <bitset>
#include <cassert>
#include <memory>
#include <mutex>
#include <optional>
#include <queue>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

#include "logger.h"
#include "util/EnumSet.h"
#include "util/math.h"

#include <d3d11.h>
#include <dxgi.h>

using namespace std::string_literals;

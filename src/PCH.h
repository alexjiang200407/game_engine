#pragma once

#ifdef _WIN32
#	define UNICODE
#	define _UNICODE
#	define WIN32_LEAN_AND_MEAN
#	include <Windows.h>
#endif

#include <bitset>
#include <cassert>
#include <memory>
#include <optional>
#include <queue>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include "logger.h"
#include "util/EnumSet.h"

#include <d3d11.h>
#include <dxgi.h>

using namespace std::string_literals;

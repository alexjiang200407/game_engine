#pragma once

#ifdef _WIN32
#	define UNICODE
#	define _UNICODE
#	define WIN32_LEAN_AND_MEAN
#	include <Windows.h>
#endif

#include <bitset>
#include <optional>
#include <queue>
#include <stdexcept>
#include <string>
#include <vector>

#include "logger.h"

using namespace std::string_literals;

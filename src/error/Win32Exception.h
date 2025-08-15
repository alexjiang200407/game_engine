#pragma once

#include <exception>
#include <string>

class Win32Exception : public std::exception
{
public:
	explicit Win32Exception(int line, const char* file, DWORD errorCode = GetLastError()) noexcept;

	const char*
	what() const noexcept override;

private:
	static std::string
	FormatErrorMessage(DWORD code);

private:
	DWORD       code;
	std::string msg;
};

#define WIN32_ERR_TEST_AND_THROW(expr)                \
	([&]() {                                          \
		SetLastError(0);                              \
		const auto res = (expr);                      \
		if (!res && GetLastError() != 0)              \
			throw Win32Exception(__LINE__, __FILE__); \
		return res;                                   \
	}())

#define WIN32_ERR_TEST_AND_EXIT(expr)    \
	([&]() {                             \
		SetLastError(0);                 \
		const auto res = (expr);         \
		if (!res && GetLastError() != 0) \
			std::exit(1);                \
		return res;                      \
	}())

#include "Win32Exception.h"

Win32Exception::Win32Exception(int line, const char* file, DWORD errorCode) noexcept :
	code(errorCode)
{
	std::ostringstream oss{};
	oss << "[File: " << file << ", Line: " << line << "] " << FormatErrorMessage(code);
	msg = oss.str();
}

const char*
Win32Exception::what() const noexcept
{
	return msg.c_str();
}

std::string
Win32Exception::FormatErrorMessage(DWORD code)
{
	LPSTR buffer = nullptr;

	DWORD size = FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr,
		code,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPSTR>(&buffer),
		0,
		nullptr);

	std::string result;
	if (size && buffer)
	{
		result.assign(buffer, size);
		LocalFree(buffer);
	}
	else
	{
		result = "Unknown error code: " + std::to_string(code);
	}

	return result;
}

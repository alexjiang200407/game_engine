#include "DXException.h"
#include <codecvt>

using namespace gfx;

namespace
{
	std::string
	WStringToString(const std::wstring& wstr)
	{
		if (wstr.empty())
			return {};
		int size_needed = WideCharToMultiByte(
			CP_UTF8,
			0,
			wstr.data(),
			(int)wstr.size(),
			nullptr,
			0,
			nullptr,
			nullptr);
		std::string result(size_needed, 0);
		WideCharToMultiByte(
			CP_UTF8,
			0,
			wstr.data(),
			(int)wstr.size(),
			result.data(),
			size_needed,
			nullptr,
			nullptr);
		return result;
	}

	std::string
	GetHRESULTErrorMessage(HRESULT hr)
	{
		LPWSTR buffer = nullptr;
		DWORD  size   = FormatMessageW(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr,
            static_cast<DWORD>(hr),
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            reinterpret_cast<LPWSTR>(&buffer),
            0,
            nullptr);

		std::wstring wmsg;
		if (size && buffer)
		{
			wmsg.assign(buffer, size);
			LocalFree(buffer);
		}
		else
		{
			wmsg = L"Unknown HRESULT: 0x" + std::to_wstring(hr);
		}
		return "Graphics Error!\n "s + WStringToString(wmsg);
	}
}

DirectXHRESULTException::DirectXHRESULTException(HRESULT hr, int line, const char* file) noexcept :
	DirectXException(line, file, GetHRESULTErrorMessage(hr))
{}

gfx::DirectXException::DirectXException(
	int                             line,
	const char*                     file,
	std::optional<std::string_view> before) noexcept
{
	std::ostringstream oss;
	FormatErrorMessage(oss, file, line, before);
	msg = oss.str();
}

const char*
gfx::DirectXException::what() const noexcept
{
	return msg.c_str();
}

void
gfx::DirectXException::FormatErrorMessage(
	std::ostream&                   os,
	const char*                     file,
	int                             line,
	std::optional<std::string_view> before)
{
	os << file << ":" << line;

	if (before)
		os << '\n' << *before;

#ifdef DEBUG

	const auto messages = DXGIInfoManager::GetSingleton().GetMessages();

	if (messages.empty())
		return;

	os << "\nReason:\n";
	for (const auto& message : messages)
	{
		os << message << '\n';
	}

#endif
}

#include "GFXException.h"

using namespace gfx;

namespace
{
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
		return "Graphics Error!\n "s + std::string(wmsg.begin(), wmsg.end());
	}
}

DirectXHRESULTException::DirectXHRESULTException(
	HRESULT          hr,
	int              line,
	const char*      file,
	DXGIInfoManager& dxgiInfoManager) noexcept :
	DirectXException(line, file, dxgiInfoManager, GetHRESULTErrorMessage(hr))
{}

gfx::DirectXException::DirectXException(
	int                             line,
	const char*                     file,
	DXGIInfoManager&                dxgiInfoManager,
	std::optional<std::string_view> before) noexcept
{
	std::ostringstream oss;
	FormatErrorMessage(oss, file, line, before, dxgiInfoManager);
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
	std::optional<std::string_view> before,
	DXGIInfoManager&                dxgiInfoManager)
{
	os << file << ":" << line;

	if (before)
		os << '\n' << *before;

	const auto messages = dxgiInfoManager.GetMessages();

	if (messages.empty())
		return;

	os << "\nReason:\n";
	for (const auto& message : messages)
	{
		os << message << '\n';
	}
}

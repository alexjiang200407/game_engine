#include "GFXException.h"

using namespace gfx;

GFXException::GFXException(
	HRESULT          hr,
	int              line,
	const char*      file,
	DXGIInfoManager* dxgiInfoManager) noexcept
{
	msg = FormatErrorMessage(hr, file, line, dxgiInfoManager);
}

const char*
gfx::GFXException::what() const noexcept
{
	return msg.c_str();
}

std::string
GFXException::FormatErrorMessage(
	HRESULT          hr,
	const char*      file,
	int              line,
	DXGIInfoManager* dxgiInfoManager)
{
	LPWSTR buffer = nullptr;
	DWORD  size   = FormatMessageW(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
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

	std::ostringstream oss;
	oss << file << ":" << line << "\nGraphics Error!\n" << std::string(wmsg.begin(), wmsg.end());

	if (dxgiInfoManager)
	{
		oss << "\nDescription:\n";
		const auto messages = dxgiInfoManager->GetMessages();
		for (const auto& message : messages)
		{
			oss << message << '\n';
		}
	}

	return oss.str();
}

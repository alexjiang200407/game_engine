#include "gfx/DXGIInfoManager.h"
#include "gfx/GFXException.h"
#include "window/Win32Exception.h"
#include <dxgidebug.h>
#include <memory>

gfx::DXGIInfoManager::DXGIInfoManager()
{
	typedef HRESULT(WINAPI * DXGIGetDebugInterface)(REFIID, void**);

	const auto hModDxgiDebug = WIN32_ERR_TEST_AND_THROW(
		LoadLibraryEx(L"dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32));

	const auto DxgiGetDebugInterface =
		WIN32_ERR_TEST_AND_THROW(reinterpret_cast<DXGIGetDebugInterface>(
			reinterpret_cast<void*>(GetProcAddress(hModDxgiDebug, "DXGIGetDebugInterface"))));
	DxgiGetDebugInterface(IID_PPV_ARGS(&pDxgiInfoQueue));
}

void
gfx::DXGIInfoManager::Set()
{
	std::lock_guard<std::mutex> lock(mtx);
	next = pDxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
}

std::vector<std::string>
gfx::DXGIInfoManager::GetMessages()
{
	std::lock_guard<std::mutex> lock(mtx);

	std::vector<std::string> messages;
	const auto               end = pDxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
	for (auto i = next; i < end; i++)
	{
		SIZE_T messageLength{};
		WIN32_ERR_TEST_AND_THROW(
			pDxgiInfoQueue->GetMessage(DXGI_DEBUG_ALL, i, nullptr, &messageLength));
		auto bytes    = std::make_unique<byte[]>(messageLength);
		auto pMessage = reinterpret_cast<DXGI_INFO_QUEUE_MESSAGE*>(bytes.get());
		WIN32_ERR_TEST_AND_THROW(
			pDxgiInfoQueue->GetMessage(DXGI_DEBUG_ALL, i, pMessage, &messageLength));
		messages.emplace_back(pMessage->pDescription);
	}
	return messages;
}

bool
gfx::DXGIInfoManager::Empty()
{
	std::lock_guard<std::mutex> lock(mtx);
	return pDxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL) == next;
}

gfx::DXGIInfoManager&
gfx::DXGIInfoManager::GetSingleton()
{
#ifdef __clang__
	[[clang::always_destroy]]
#endif
	static DXGIInfoManager singleton{};
	return singleton;
}

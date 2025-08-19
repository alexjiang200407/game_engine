#pragma once
#include <dxgidebug.h>
#include <mutex>
#include <wrl.h>

namespace gfx
{
	class DXGIInfoManager
	{
	private:
		DXGIInfoManager();

	public:
		~DXGIInfoManager()                      = default;
		DXGIInfoManager(const DXGIInfoManager&) = delete;

		DXGIInfoManager&
		operator=(const DXGIInfoManager&) = delete;

		void
		Set();

		std::vector<std::string>
		GetMessages();

		bool
		Empty();

		static DXGIInfoManager&
		GetSingleton();

	private:
		unsigned long long                     next = 0u;
		Microsoft::WRL::ComPtr<IDXGIInfoQueue> pDxgiInfoQueue;
		std::mutex                             mtx;
	};
}

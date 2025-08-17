#pragma once
#include <dxgidebug.h>
#include <wrl.h>

namespace gfx
{
	class DXGIInfoManager
	{
	public:
		DXGIInfoManager();
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

	private:
		unsigned long long                     next = 0u;
		Microsoft::WRL::ComPtr<IDXGIInfoQueue> pDxgiInfoQueue;
		std::mutex                             mtx;
	};
}

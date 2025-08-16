#include "gfx/DXGIInfoManager.h"
#include <exception>
#include <iostream>
#include <sstream>
#include <string>

namespace gfx
{
	class GFXException : public std::exception
	{
	public:
		explicit GFXException(
			HRESULT          a_hr,
			int              line,
			const char*      file,
			DXGIInfoManager* dxgiInfoManager = nullptr) noexcept;

		const char*
		what() const noexcept override;

	private:
		static std::string
		FormatErrorMessage(
			HRESULT          a_hr,
			const char*      file,
			int              line,
			DXGIInfoManager* dxgiInfoManager);

		std::string msg;
	};
}

#define GFX_ERROR_TEST_AND_THROW(expr, dxgiInfoManager)                           \
	([&]() {                                                                      \
		if (dxgiInfoManager)                                                      \
			dxgiInfoManager->Set();                                               \
		HRESULT hr = (expr);                                                      \
		if (FAILED(hr))                                                           \
			throw gfx::GFXException(hr, __LINE__, __REL_FILE__, dxgiInfoManager); \
		return hr;                                                                \
	}())

#define GFX_ERROR_TEST_AND_EXIT(expr, dxgiInfoManager)                                         \
	([&]() {                                                                                   \
		if (dxgiInfoManager)                                                                   \
			dxgiInfoManager->Set();                                                            \
		HRESULT hr = (expr);                                                                   \
		if (FAILED(hr))                                                                        \
		{                                                                                      \
			std::cerr << gfx::GFXException(hr, __LINE__, __REL_FILE__, dxgiInfoManager).what() \
					  << "\n";                                                                 \
			std::exit(1);                                                                      \
		}                                                                                      \
		return hr;                                                                             \
	}())

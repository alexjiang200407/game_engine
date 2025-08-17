#include "gfx/DXGIInfoManager.h"
#include <exception>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>

namespace gfx
{
	class DirectXException : public std::exception
	{
	public:
		DirectXException(
			int                             line,
			const char*                     file,
			DXGIInfoManager&                dxgiInfoManager,
			std::optional<std::string_view> before = std::nullopt) noexcept;

		const char*
		what() const noexcept override;

	private:
		static void
		FormatErrorMessage(
			std::ostream&                   os,
			const char*                     file,
			int                             line,
			std::optional<std::string_view> before,
			DXGIInfoManager&                dxgiInfoManager);

		std::string msg;
	};

	class DirectXHRESULTException : public DirectXException
	{
	public:
		explicit DirectXHRESULTException(
			HRESULT          a_hr,
			int              line,
			const char*      file,
			DXGIInfoManager& dxgiInfoManager) noexcept;
	};
}

#define GFX_ERROR_TEST_AND_THROW(expr, dxgiInfoManager)                                      \
	([&]() {                                                                                 \
		dxgiInfoManager.Set();                                                               \
		HRESULT hr = (expr);                                                                 \
		if (FAILED(hr))                                                                      \
			throw gfx::DirectXHRESULTException(hr, __LINE__, __REL_FILE__, dxgiInfoManager); \
		return hr;                                                                           \
	}())

#define GFX_ERROR_TEST_AND_EXIT(expr, dxgiInfoManager)                                             \
	([&]() {                                                                                       \
		dxgiInfoManager.Set();                                                                     \
		HRESULT hr = (expr);                                                                       \
		if (FAILED(hr))                                                                            \
		{                                                                                          \
			std::cerr << gfx::DirectXHRESULTException(hr, __LINE__, __REL_FILE__, dxgiInfoManager) \
							 .what()                                                               \
					  << "\n";                                                                     \
			std::exit(1);                                                                          \
		}                                                                                          \
		return hr;                                                                                 \
	}())

#define GFX_ERROR(expr, dxgiInfoManager) \
	dxgiInfoManager.Set();               \
	expr;                                \
	if (!dxgiInfoManager.Empty())        \
	throw gfx::DirectXException(__LINE__, __REL_FILE__, dxgiInfoManager)\

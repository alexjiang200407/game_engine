#include <exception>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>

#ifdef DEBUG
#	include "gfx/DXGIInfoManager.h"
#endif

namespace gfx
{
	class DirectXException : public std::exception
	{
	public:
		DirectXException(
			int                             line,
			const char*                     file,
			std::optional<std::string_view> before = std::nullopt) noexcept;

		const char*
		what() const noexcept override;

	private:
		static void
		FormatErrorMessage(
			std::ostream&                   os,
			const char*                     file,
			int                             line,
			std::optional<std::string_view> before);

		std::string msg;
	};

	class DirectXHRESULTException : public DirectXException
	{
	public:
		explicit DirectXHRESULTException(HRESULT a_hr, int line, const char* file) noexcept;
	};
}

#ifdef DEBUG

#	define DX_HR_ERROR_TEST_AND_THROW(expr)                                      \
		([&]() {                                                                \
			gfx::DXGIInfoManager::GetSingleton().Set();                         \
			HRESULT hr = (expr);                                                \
			if (FAILED(hr))                                                     \
				throw gfx::DirectXHRESULTException(hr, __LINE__, __REL_FILE__); \
			return hr;                                                          \
		}())

#	define DX_HR_ERROR_TEST_AND_EXIT(expr)                                                    \
		([&]() {                                                                             \
			gfx::DXGIInfoManager::GetSingleton().Set();                                      \
			HRESULT hr = (expr);                                                             \
			if (FAILED(hr))                                                                  \
			{                                                                                \
				std::cerr << gfx::DirectXHRESULTException(hr, __LINE__, __REL_FILE__).what() \
						  << "\n";                                                           \
				std::exit(1);                                                                \
			}                                                                                \
			return hr;                                                                       \
		}())

#	define DX_CALL(expr)                                \
		gfx::DXGIInfoManager::GetSingleton().Set();        \
		expr;                                              \
		if (!gfx::DXGIInfoManager::GetSingleton().Empty()) \
		throw gfx::DirectXException(__LINE__, __REL_FILE__)

#else

#	define DX_HR_ERROR_TEST_AND_THROW(expr)                                       \
		([&]() {                                                                \
			HRESULT hr = (expr);                                                \
			if (FAILED(hr))                                                     \
				throw gfx::DirectXHRESULTException(hr, __LINE__, __REL_FILE__); \
			return hr;                                                          \
		}())

#	define DX_HR_ERROR_TEST_AND_EXIT(expr) \
		([&]() {                         \
			HRESULT hr = (expr);         \
			if (FAILED(hr))              \
			{                            \
				std::exit(1);            \
			}                            \
			return hr;                   \
		}())

#	define DX_CALL(expr) expr

#endif

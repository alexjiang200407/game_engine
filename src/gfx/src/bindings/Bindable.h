#pragma once
#include "DX11Graphics.h"

namespace gfx
{
	class DX11Graphics;

	template <typename T>
	constexpr size_t
	type_tag()
	{
		return reinterpret_cast<size_t>(&type_tag<T>);
	}

	class IBindable
	{
	public:
		virtual ~IBindable() noexcept = default;

		virtual void
		Bind(DX11Graphics& gfx) = 0;

	protected:
		static ID3D11DeviceContext*
		GetContext(DX11Graphics& gfx) noexcept
		{
			assert(gfx.pContext.Get());
			return gfx.pContext.Get();
		}

		static ID3D11Device*
		GetDevice(DX11Graphics& gfx) noexcept
		{
			assert(gfx.pDevice.Get());
			return gfx.pDevice.Get();
		}
	};

	template <typename Super, typename... Args>
	class Bindable : public IBindable
	{
	public:
		using UIDArgs = std::tuple<Args...>;

		Bindable(Args... uidArgs) : uid(GenerateUID(std::forward<Args>(uidArgs)...)) {}

		static std::string
		GenerateUID(Args... args)
		{
			std::ostringstream oss;
			oss << std::hex << type_tag<Super>();
			((oss << '#' << StreamArg(std::forward<Args>(args))), ...);
			return oss.str();
		}

	private:
		template <typename T>
		static auto
		StreamArg(T&& arg)
		{
			if constexpr (
				std::is_same_v<std::decay_t<T>, std::wstring> ||
				std::is_same_v<std::decay_t<T>, std::wstring_view> ||
				std::is_same_v<std::decay_t<T>, const wchar_t*>)
			{
				return util::wtos(arg);
			}
			else
			{
				return std::forward<T>(arg);
			}
		}

	private:
		std::string uid;
	};
}

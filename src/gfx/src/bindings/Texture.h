#pragma once
#include "bindings/Bindable.h"

namespace gfx
{
	class DX11Graphics;

	class Texture : public Bindable<Texture, std::wstring_view>
	{
	public:
		enum class Format
		{
			kPNG,
			kJPG,
			kDDS,
			kInvalid
		};

		enum class Slot : unsigned int
		{
			kDiffuse = 0u,
			kSpecular,
			kNormal
		};

	public:
		Texture(DX11Graphics& gfx, const std::string& str, Format format, Slot slot);
		Texture(DX11Graphics& gfx, const std::wstring& ws, Format format, Slot slot);

		void
		Bind(DX11Graphics& gfx) override;

		bool
		HasAlpha() const noexcept;

	private:
		Slot                                             slot;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureSRV;
		bool                                             hasAlpha = false;
	};
}
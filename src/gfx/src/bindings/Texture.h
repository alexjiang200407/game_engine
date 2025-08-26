#pragma once
#include "bindings/Bindable.h"

namespace gfx
{
	class DX11Graphics;

	class Texture : public Bindable
	{
	public:
		enum class Format
		{
			kPNG,
			kJPG,
			kDDS,
			kInvalid
		};

	public:
		Texture(DX11Graphics& gfx, const std::wstring& ws, Format format);
		Texture(DX11Graphics& gfx, std::span<const unsigned char> bytes, Format format);

		void
		Bind(DX11Graphics& gfx) override;

	private:
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureSRV;
	};
}
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
			kDDS,
			kInvalid
		};

	public:
		Texture(DX11Graphics& gfx, const std::wstring& ws, Format format);

		void
		Bind(DX11Graphics& gfx) override;

	private:
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureSRV;
	};
}
#pragma once
#include "bindings/Bindable.h"

namespace gfx
{
	class Graphics;

	class Texture : Bindable
	{
	public:
		enum class Format
		{
			kPNG,
			kDDS,
			kInvalid
		};

	public:
		Texture(Graphics& gfx, const std::wstring& ws, Format format);

		void
		Bind(Graphics& gfx) override;

	private:
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureSRV;
	};
}
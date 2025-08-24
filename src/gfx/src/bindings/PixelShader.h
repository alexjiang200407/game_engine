#pragma once
#include "bindings/Bindable.h"

namespace gfx
{
	class DX11Graphics;

	class PixelShader : public Bindable
	{
	public:
		PixelShader(DX11Graphics& gfx, const std::wstring& path);

		void
		Bind(DX11Graphics& gfx) override;

	protected:
		Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
	};
}
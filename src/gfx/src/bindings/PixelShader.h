#pragma once
#include "bindings/Bindable.h"

namespace gfx
{
	class DX11Graphics;

	class PixelShader : public Bindable<PixelShader, const wchar_t*>
	{
	public:
		PixelShader(DX11Graphics& gfx, const wchar_t* path);

		void
		Bind(DX11Graphics& gfx) override;

	protected:
		Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
	};
}
#pragma once
#include "bindings/Bindable.h"

namespace gfx
{
	class Graphics;

	class PixelShader : public Bindable
	{
	public:
		PixelShader(Graphics& gfx, const std::wstring& path);

		void
		Bind(Graphics& gfx) override;

	protected:
		Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
	};
}
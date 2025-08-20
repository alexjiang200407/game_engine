#pragma once
#include "gfx/bind/Bindable.h"

namespace gfx
{
	class Graphics;

	class PixelShader : public Bindable
	{
	public:
		PixelShader(Graphics& gfx, const std::wstring& path);

		void
		Bind(Graphics& gfx) noexcept(!DEBUG) override;

	protected:
		Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
	};
}
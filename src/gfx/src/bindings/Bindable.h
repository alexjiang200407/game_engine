#pragma once

namespace gfx
{
	class DX11Graphics;

	class Bindable
	{
	public:
		virtual ~Bindable() noexcept = default;

		virtual void
		Bind(DX11Graphics& gfx) = 0;

	protected:
		static ID3D11DeviceContext*
		GetContext(DX11Graphics& gfx) noexcept;

		static ID3D11Device*
		GetDevice(DX11Graphics& gfx) noexcept;
	};
}

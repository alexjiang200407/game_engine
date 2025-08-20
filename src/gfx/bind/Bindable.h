#pragma once

namespace gfx
{
	class Graphics;

	class Bindable
	{
	public:
		virtual ~Bindable() noexcept = default;

		virtual void
		Bind(Graphics& gfx) noexcept(!DEBUG) = 0;

	protected:
		static ID3D11DeviceContext*
		GetContext(Graphics& gfx) noexcept;

		static ID3D11Device*
		GetDevice(Graphics& gfx) noexcept;
	};
}

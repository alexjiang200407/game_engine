#pragma once

namespace gfx
{
	class DX11Graphics;

	class Graphics
	{
	public:
		Graphics();
		Graphics(const Graphics&) = delete;
		~Graphics() noexcept;

		Graphics&
		operator=(const Graphics&) = delete;

		void
		StartFrame() const;

		void
		EndFrame() const;

		void
		ClearBuffer(float red, float green, float blue) const;

		void
		SetProjection(DirectX::FXMMATRIX proj) noexcept;

		DirectX::XMMATRIX
		GetCamera() const noexcept;

		void
		SetCamera(DirectX::XMMATRIX camera) noexcept;

		DX11Graphics*
		operator->() noexcept;

		DX11Graphics&
		operator*() noexcept;

		const DX11Graphics&
		operator*() const noexcept;

	private:
		std::unique_ptr<gfx::DX11Graphics> pImpl;
	};
}
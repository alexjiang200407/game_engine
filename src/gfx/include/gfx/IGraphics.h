#pragma once

namespace gfx
{
	class IGraphics
	{
	public:
		enum class RenderAPI
		{
			kDX11,
			kInvalid,
		};

	public:
		IGraphics()                 = default;
		IGraphics(const IGraphics&) = delete;
		virtual ~IGraphics()        = default;

		IGraphics&
		operator=(const IGraphics&) = delete;

		virtual void
		StartFrame() const = 0;

		virtual void
		EndFrame() const = 0;

		virtual void
		ClearBuffer(float red, float green, float blue) const = 0;

		virtual void
		SetProjection(DirectX::FXMMATRIX proj) noexcept = 0;

		static std::unique_ptr<IGraphics>
		Make(unsigned int width, unsigned int height);

		virtual RenderAPI
		GetRenderAPI() const noexcept = 0;

	private:
		RenderAPI api;
	};
}
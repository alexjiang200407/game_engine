#pragma once
#include "bindings/Bindable.h"
#include "dcbuffer/DynamicConstant.h"

namespace gfx
{
	class PixelConstantBufferEX : public Bindable<PixelConstantBufferEX, std::string_view>
	{
	public:
		void
		Update(DX11Graphics& gfx, const dcb::Buffer& buf) const;

		void
		Bind(DX11Graphics& gfx) override;

		virtual const dcb::LayoutElement&
		GetRootLayoutElement() const noexcept = 0;

	protected:
		PixelConstantBufferEX(
			DX11Graphics&             gfx,
			const dcb::LayoutElement& layoutRoot,
			std::string_view          tag,
			UINT                      slot,
			const dcb::Buffer*        pBuf);

	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;
		UINT                                 slot;
	};

	class CachingPixelConstantBufferEX : public PixelConstantBufferEX
	{
	public:
		CachingPixelConstantBufferEX(
			DX11Graphics&            gfx,
			const dcb::CookedLayout& layout,
			std::string_view         tag,
			UINT                     slot);

		CachingPixelConstantBufferEX(
			DX11Graphics&      gfx,
			std::string_view   tag,
			const dcb::Buffer& buf,
			UINT               slot);

		const dcb::LayoutElement&
		GetRootLayoutElement() const noexcept override;

		const dcb::Buffer&
		GetBuffer() const noexcept;

		void
		SetBuffer(const dcb::Buffer& buf_in)
		{
			buf.CopyFrom(buf_in);
			dirty = true;
		}

		void
		Bind(DX11Graphics& gfx) noexcept override;

	private:
		bool        dirty = false;
		dcb::Buffer buf;
	};

	class NocachePixelConstantBufferEX : public PixelConstantBufferEX
	{
	public:
		NocachePixelConstantBufferEX(
			DX11Graphics&            gfx,
			const dcb::CookedLayout& layout,
			std::string_view         tag,
			UINT                     slot);

		NocachePixelConstantBufferEX(
			DX11Graphics&      gfx,
			const dcb::Buffer& buf,
			std::string_view   tag,
			UINT               slot);

		const dcb::LayoutElement&
		GetRootLayoutElement() const noexcept override;

	private:
		std::shared_ptr<dcb::LayoutElement> pLayoutRoot;
	};
}
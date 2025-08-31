#pragma once
#include "bindings/Bindable.h"
#include <vector>

namespace gfx
{
	class DX11Graphics;

	class IndexBuffer : public Bindable<IndexBuffer, std::string_view, std::string_view>
	{
	public:
		IndexBuffer(
			DX11Graphics&                      gfx,
			const std::vector<unsigned short>& indices,
			std::string_view                   fileName,
			std::string_view                   tag = ""sv);

		void
		Bind(DX11Graphics& gfx) override;

		UINT
		GetCount() const noexcept;

	protected:
		UINT                                 count;
		Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer;
	};
}

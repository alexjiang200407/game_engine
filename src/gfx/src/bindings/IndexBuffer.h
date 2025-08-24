#pragma once
#include "bindings/Bindable.h"
#include <vector>

namespace gfx
{
	class DX11Graphics;

	class IndexBuffer : public Bindable
	{
	public:
		IndexBuffer(DX11Graphics& gfx, const std::vector<unsigned short>& indices);

		void
		Bind(DX11Graphics& gfx) override;

		UINT
		GetCount() const noexcept;

	protected:
		UINT                                 count;
		Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer;
	};
}

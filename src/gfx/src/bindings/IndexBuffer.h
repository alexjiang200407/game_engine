#pragma once
#include "bindings/Bindable.h"
#include <vector>

namespace gfx
{
	class Graphics;

	class IndexBuffer : public Bindable
	{
	public:
		IndexBuffer(Graphics& gfx, const std::vector<unsigned short>& indices);

		void
		Bind(Graphics& gfx) override;

		UINT
		GetCount() const noexcept;

	protected:
		UINT                                 count;
		Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer;
	};
}

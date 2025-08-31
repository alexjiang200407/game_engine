#pragma once
#include "bindings/Bindable.h"
#include "geom/Vertex.h"

namespace gfx
{
	class DX11Graphics;

	class InputLayout : public Bindable<InputLayout, const geom::VertexLayout&>
	{
	public:
		InputLayout(
			DX11Graphics&             gfx,
			const geom::VertexLayout& layout,
			ID3DBlob*                 pVertexShaderBytecode);

		void
		Bind(DX11Graphics& gfx) override;

	protected:
		Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
	};
}

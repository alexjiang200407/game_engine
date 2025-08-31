#pragma once
#include "bindings/Bindable.h"

namespace gfx
{
	class DX11Graphics;

	class VertexShader : public Bindable<VertexShader, const wchar_t*>
	{
	public:
		VertexShader(DX11Graphics& gfx, const wchar_t* path);

		void
		Bind(DX11Graphics& gfx) override;

		ID3DBlob*
		GetBytecode() const noexcept;

	protected:
		Microsoft::WRL::ComPtr<ID3DBlob>           pBytecodeBlob;
		Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
	};
}

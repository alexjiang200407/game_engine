#pragma once
#include "bindings/Bindable.h"

namespace gfx
{
	class DX11Graphics;

	class VertexShader : public Bindable
	{
	public:
		VertexShader(DX11Graphics& gfx, const std::wstring& path);

		void
		Bind(DX11Graphics& gfx) override;

		ID3DBlob*
		GetBytecode() const noexcept;

	protected:
		Microsoft::WRL::ComPtr<ID3DBlob>           pBytecodeBlob;
		Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
	};
}

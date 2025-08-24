#include "bindings/PixelShader.h"

gfx::PixelShader::PixelShader(DX11Graphics& gfx, const std::wstring& path)
{
	Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
	DX_HR_ERROR_TEST_AND_THROW(D3DReadFileToBlob(path.c_str(), &pBlob));
	DX_HR_ERROR_TEST_AND_THROW(GetDevice(gfx)->CreatePixelShader(
		pBlob->GetBufferPointer(),
		pBlob->GetBufferSize(),
		nullptr,
		&pPixelShader));
}

void
gfx::PixelShader::Bind(DX11Graphics& gfx)
{
	DX_CALL(GetContext(gfx)->PSSetShader(pPixelShader.Get(), nullptr, 0u));
}

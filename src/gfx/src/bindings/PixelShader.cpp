#include "gfx/bindings/PixelShader.h"
#include "gfx/GFXException.h"

gfx::PixelShader::PixelShader(Graphics& gfx, const std::wstring& path)
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
gfx::PixelShader::Bind(Graphics& gfx)
{
	DX_CALL(GetContext(gfx)->PSSetShader(pPixelShader.Get(), nullptr, 0u));
}

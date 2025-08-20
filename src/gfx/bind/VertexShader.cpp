#include "gfx/bind/VertexShader.h"
#include "gfx/GFXException.h"

gfx::VertexShader::VertexShader(Graphics& gfx, const std::wstring& path)
{
	DX_HR_ERROR_TEST_AND_THROW(D3DReadFileToBlob(path.c_str(), &pBytecodeBlob));
	DX_HR_ERROR_TEST_AND_THROW(GetDevice(gfx)->CreateVertexShader(
		pBytecodeBlob->GetBufferPointer(),
		pBytecodeBlob->GetBufferSize(),
		nullptr,
		&pVertexShader));
}

void
gfx::VertexShader::Bind(Graphics& gfx) noexcept(!DEBUG)
{
	DX_CALL(GetContext(gfx)->VSSetShader(pVertexShader.Get(), nullptr, 0u));
}

ID3DBlob*
gfx::VertexShader::GetBytecode() const noexcept
{
	return pBytecodeBlob.Get();
}
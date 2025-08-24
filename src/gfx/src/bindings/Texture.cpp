#include "bindings/Texture.h"
#include <DirectXTex.h>

gfx::Texture::Texture(DX11Graphics& gfx, const std::wstring& ws, Format format)
{
	namespace dx  = DirectX;
	namespace wrl = Microsoft::WRL;

	wrl::ComPtr<ID3D11Resource> texture;
	{
		dx::ScratchImage image;

		switch (format)
		{
		case Format::kDDS:
			DX_HR_ERROR_TEST_AND_THROW(
				dx::LoadFromDDSFile(ws.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, image));
			break;
		case Format::kPNG:
			DX_HR_ERROR_TEST_AND_THROW(
				dx::LoadFromWICFile(ws.c_str(), DirectX::WIC_FLAGS_NONE, nullptr, image));
			break;
		default:
			throw std::runtime_error("Invalid format");
		}

		DX_HR_ERROR_TEST_AND_THROW(dx::CreateTexture(
			GetDevice(gfx),
			image.GetImages(),
			image.GetImageCount(),
			image.GetMetadata(),
			&texture));
	}

	DX_HR_ERROR_TEST_AND_THROW(
		GetDevice(gfx)->CreateShaderResourceView(texture.Get(), nullptr, &pTextureSRV));
}

void
gfx::Texture::Bind(DX11Graphics& gfx)
{
	DX_CALL(GetContext(gfx)->PSSetShaderResources(0u, 1u, pTextureSRV.GetAddressOf()));
}

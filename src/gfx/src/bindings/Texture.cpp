#include "bindings/Texture.h"
#include <DirectXTex.h>
#include <util/str.h>

namespace wrl = Microsoft::WRL;

gfx::Texture::Texture(DX11Graphics& gfx, const std::string& str, Format format, Slot a_slot) :
	Texture(gfx, util::stow(str), format, a_slot)
{}

gfx::Texture::Texture(DX11Graphics& gfx, const std::wstring& ws, Format format, Slot a_slot) :
	Bindable(ws), slot(a_slot)
{
	namespace dx = DirectX;

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
		case Format::kJPG:
			DX_HR_ERROR_TEST_AND_THROW(
				dx::LoadFromWICFile(ws.c_str(), DirectX::WIC_FLAGS_IGNORE_SRGB, nullptr, image));
			break;
		default:
			throw std::runtime_error("Invalid format");
		}

		if (image.GetImage(0, 0, 0)->format != DXGI_FORMAT_B8G8R8A8_UNORM)
		{
			dx::ScratchImage converted;
			DX_HR_ERROR_TEST_AND_THROW(dx::Convert(
				*image.GetImage(0, 0, 0),
				DXGI_FORMAT_B8G8R8A8_UNORM,
				DirectX::TEX_FILTER_DEFAULT,
				DirectX::TEX_THRESHOLD_DEFAULT,
				converted));
			image = std::move(converted);
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
	DX_CALL(GetContext(gfx)
	            ->PSSetShaderResources(static_cast<UINT>(slot), 1u, pTextureSRV.GetAddressOf()));
}

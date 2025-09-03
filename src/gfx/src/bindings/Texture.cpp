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

		DX_HR_ERROR_TEST_AND_THROW(dx::CreateTexture(
			GetDevice(gfx),
			image.GetImages(),
			image.GetImageCount(),
			image.GetMetadata(),
			&texture));

		hasAlpha = DirectX::HasAlpha(image.GetMetadata().format) &&
		           (!util::Settings::Module("Graphics").Get("bExpensiveCheckAlpha", true) ||
		            !image.IsAlphaAllOpaque());
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

bool
gfx::Texture::HasAlpha() const noexcept
{
	return hasAlpha;
}

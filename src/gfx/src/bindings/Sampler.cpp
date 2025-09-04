#include "bindings/Sampler.h"

gfx::Sampler::Sampler(DX11Graphics& gfx)
{
	auto textureCfg = util::Settings::Module("Textures");

	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter             = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU           = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV           = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW           = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias         = 0.0f;
	samplerDesc.MinLOD             = 0.0f;
	samplerDesc.MaxLOD             = D3D11_FLOAT32_MAX;
	samplerDesc.MaxAnisotropy      = std::min(
        static_cast<unsigned int>(D3D11_REQ_MAXANISOTROPY),
        textureCfg.Get("uMaxAnisotropy", 16u));

	DX_HR_ERROR_TEST_AND_THROW(GetDevice(gfx)->CreateSamplerState(&samplerDesc, &pSampler));
}

void
gfx::Sampler::Bind(DX11Graphics& gfx)
{
	GetContext(gfx)->PSSetSamplers(0, 1, pSampler.GetAddressOf());
}

#include "DX11Graphics.h"
#include <gfx/Graphics.h>

gfx::Graphics::Graphics(unsigned int width, unsigned int height) :
	pImpl(std::make_unique<DX11Graphics>(width, height))
{
	assert(pImpl);
}

gfx::Graphics::~Graphics() {}

void
gfx::Graphics::StartFrame() const
{
	pImpl->StartFrame();
}

void
gfx::Graphics::EndFrame() const
{
	pImpl->EndFrame();
}

void
gfx::Graphics::ClearBuffer(float red, float green, float blue) const
{
	pImpl->ClearBuffer(red, green, blue);
}

void
gfx::Graphics::SetProjection(DirectX::FXMMATRIX proj) noexcept
{
	pImpl->SetProjection(proj);
}

DirectX::XMMATRIX
gfx::Graphics::GetCamera() const noexcept
{
	return pImpl->GetCamera();
}

void
gfx::Graphics::SetCamera(DirectX::XMMATRIX camera) noexcept
{
	pImpl->SetCamera(camera);
}

gfx::DX11Graphics*
gfx::Graphics::operator->() noexcept
{
	return pImpl.get();
}

gfx::DX11Graphics&
gfx::Graphics::operator*() noexcept
{
	return *pImpl;
}

const gfx::DX11Graphics&
gfx::Graphics::operator*() const noexcept
{
	return *pImpl;
}

void
gfx::Graphics::ResizeBuffers(unsigned int width, unsigned int height)
{
	pImpl->ResizeBuffers(width, height);
}
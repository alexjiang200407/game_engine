#pragma once
#include "draw/Drawable.h"

namespace gfx
{
	class SolidSphere : public Drawable
	{
	public:
		SolidSphere(DX11Graphics& gfx, float radius);

		void
		Update(float dt) noexcept override;

		void
		SetPos(DirectX::XMFLOAT3 pos) noexcept;

		DirectX::XMMATRIX
		GetTransformXM() const noexcept override;

	private:
		DirectX::XMFLOAT3 pos = { 1.0f, 1.0f, 1.0f };
	};
}
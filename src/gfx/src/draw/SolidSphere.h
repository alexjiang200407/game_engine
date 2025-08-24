#pragma once
#include "draw/DrawableBase.h"

namespace gfx
{
	class SolidSphere : public DrawableBase<SolidSphere>
	{
	public:
		SolidSphere(DX11Graphics& gfx, float radius);

		void
		Update(float dt) noexcept override;

		void
		SetPos(DirectX::XMFLOAT3 pos) noexcept;

		DirectX::XMMATRIX
		GetTransformXM() const noexcept override;

		static void
		StaticBindingsConstructor(DX11Graphics& gfx, DrawableBase<SolidSphere>& solidSphereBase);

	private:
		DirectX::XMFLOAT3 pos = { 1.0f, 1.0f, 1.0f };
	};
}
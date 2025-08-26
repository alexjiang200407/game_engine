#pragma once
#include "draw/DrawableBase.h"

namespace gfx
{
	class DX11Graphics;
	class Mesh : public DrawableBase<Mesh>
	{
	public:
		Mesh(
			DX11Graphics&                          gfx,
			const std::string&                     path,
			DirectX::XMFLOAT3                      material,
			std::mt19937&                          rng,
			std::uniform_real_distribution<float>& adist,
			std::uniform_real_distribution<float>& ddist,
			std::uniform_real_distribution<float>& odist,
			std::uniform_real_distribution<float>& rdist,
			std::uniform_real_distribution<float>& bdist,
			float                                  scale = 1.0f);

		static void
		StaticBindingsConstructor(DX11Graphics& gfx, DrawableBase<Mesh>& meshBase);

		void
		Update(float dt) noexcept override;

		DirectX::XMMATRIX
		GetTransformXM() const noexcept override;

	private:
		float r;
		float roll  = 0.0f;
		float pitch = 0.0f;
		float yaw   = 0.0f;
		float theta;
		float phi;
		float chi;

		float droll;
		float dpitch;
		float dyaw;
		float dtheta;
		float dphi;
		float dchi;

		DirectX::XMFLOAT3X3 mt;
	};
}
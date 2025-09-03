#pragma once
#include "bindings/ConstantBuffers.h"
#include "draw/SolidSphere.h"
#include <gfx/IPointLight.h>

#ifdef _MSC_VER
#	pragma warning(push)
#	pragma warning(disable: 4324)
#endif

namespace gfx
{
	class DX11Graphics;

	class PointLight : public IPointLight
	{
	public:
		PointLight(DX11Graphics& gfx, float radius = 0.5f);

		void
		DrawControlWindow() noexcept override;

		void
		Reset() noexcept override;

		void
		Draw(Graphics& gfx) const override;

		void
		Bind(Graphics& gfx, DirectX::FXMMATRIX view) const override;

		struct PointLightCBuf
		{
			alignas(16) DirectX::XMFLOAT3 pos;
			alignas(16) DirectX::XMFLOAT3 ambient;
			alignas(16) DirectX::XMFLOAT3 diffuseColor;
			float diffuseIntensity;
			float attConst;
			float attLin;
			float attQuad;
		};

	private:
		PointLightCBuf                              cbData;
		mutable SolidSphere                         mesh;
		mutable PixelConstantBuffer<PointLightCBuf> cbuf;
	};
}

#ifdef _MSC_VER
#	pragma warning(pop)
#endif
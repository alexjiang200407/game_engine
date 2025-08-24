#pragma once

#include "bindings/Bindable.h"
#include "bindings/ConstantBuffers.h"
#include "draw/Drawable.h"
#include <DirectXMath.h>
#include <memory>

namespace gfx
{
	class DX11Graphics;
	class Drawable;

	class TransformCBuffer : public Bindable
	{
	private:
		struct Transforms
		{
			DirectX::XMMATRIX modelViewProj;
			DirectX::XMMATRIX model;
		};

	public:
		TransformCBuffer(DX11Graphics& gfx, const Drawable& parent, unsigned int slot = 0u);
		void
		Bind(DX11Graphics& gfx) override;

	private:
		static inline std::unique_ptr<VertexConstantBuffer<Transforms>> pVcbuf;
		const Drawable&                                                 parent;
	};
}
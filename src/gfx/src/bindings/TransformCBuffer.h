#pragma once

#include "bindings/Bindable.h"
#include "bindings/ConstantBuffers.h"
#include "draw/Drawable.h"
#include <DirectXMath.h>
#include <memory>

namespace gfx
{
	class Graphics;
	class Drawable;

	class TransformCBuffer : public Bindable
	{
	public:
		TransformCBuffer(Graphics& gfx, const Drawable& parent);
		void
		Bind(Graphics& gfx) override;

	private:
		static inline std::unique_ptr<VertexConstantBuffer<DirectX::XMMATRIX>> pVcbuf;
		const Drawable&                                                        parent;
	};
}
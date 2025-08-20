#pragma once

#include "gfx/bind/Bindable.h"
#include "gfx/bind/ConstantBuffers.h"
#include "gfx/draw/Drawable.h"
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
		Bind(Graphics& gfx) noexcept(!DEBUG) override;

	private:
		static inline std::unique_ptr<VertexConstantBuffer<DirectX::XMMATRIX>> pVcbuf;
		const Drawable&                                                        parent;
	};
}
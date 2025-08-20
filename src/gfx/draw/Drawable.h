#pragma once
#include "gfx/Graphics.h"
#include <DirectXMath.h>
#include <vector>

namespace gfx
{
	class Bindable;

	class Drawable
	{
		template <class T>
		friend class DrawableBase;

	public:
		Drawable()                = default;
		Drawable(const Drawable&) = delete;
		virtual ~Drawable()       = default;

		virtual DirectX::XMMATRIX
		GetTransformXM() const noexcept = 0;

		void
		Draw(Graphics& gfx) const noexcept(!DEBUG);

		virtual void
		Update(float dt) noexcept = 0;

	protected:
		void
		AddBind(std::unique_ptr<Bindable> bind) noexcept(!DEBUG);

		void
		AddIndexBuffer(std::unique_ptr<class IndexBuffer> ibuf) noexcept(!DEBUG);

	private:
		virtual const std::vector<std::unique_ptr<Bindable>>&
		GetStaticBinds() const noexcept = 0;

	private:
		const class IndexBuffer*               pIndexBuffer = nullptr;
		std::vector<std::unique_ptr<Bindable>> binds;
	};
}
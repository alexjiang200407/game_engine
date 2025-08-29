#pragma once
#include "DX11Graphics.h"
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

		Drawable&
		operator=(const Drawable&) = delete;

		virtual void
		Draw(Graphics& gfx) const;

		virtual DirectX::XMMATRIX
		GetTransformXM() const noexcept = 0;

		virtual void
		Update(float dt) noexcept = 0;

	protected:
		template <typename T, typename... Args>
		T&
		AddBind(Args&&... args)
		{
			using U = std::decay_t<T>;

			auto bind = std::make_unique<U>(std::forward<Args>(args)...);

			binds.push_back(std::move(bind));
			return *static_cast<U*>(binds.back().get());
		}

	private:
		virtual const std::vector<std::unique_ptr<Bindable>>&
		GetStaticBinds() const noexcept = 0;

	private:
		std::vector<std::unique_ptr<Bindable>> binds;
	};
}
#pragma once
#include "DX11Graphics.h"
#include "gfx/IDrawable.h"
#include <DirectXMath.h>
#include <vector>

namespace gfx
{
	class Bindable;

	class Drawable : public IDrawable
	{
		template <class T>
		friend class DrawableBase;

	public:
		Drawable()                = default;
		Drawable(const Drawable&) = delete;

		Drawable&
		operator=(const Drawable&) = delete;

		void
		Draw(Graphics& gfx) const override;

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
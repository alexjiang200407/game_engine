#pragma once
#include "DX11Graphics.h"
#include "draw/Codex.h"
#include <DirectXMath.h>
#include <vector>

namespace gfx
{
	class IBindable;

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
		Update(float) noexcept {};

	protected:
		template <typename T, typename... Args>
		T&
		AddBind(const typename T::UIDArgs& uidArgs, Args&&... creationArgs)
		{
			auto uid = std::apply(
				[](auto&&... args) {
					return T::GenerateUID(std::forward<decltype(args)>(args)...);
				},
				uidArgs);
			auto bind = Codex::Get(uid);

			if (!bind)
			{
				bind = Codex::Insert<T>(uid, std::forward<Args>(creationArgs)...);
			}

			binds.push_back(std::move(bind));
			return *static_cast<T*>(binds.back().get());
		}

		template <typename T, typename... Args>
		T&
		AddUniqueBind(Args&&... args)
		{
			auto bind = std::make_shared<T>(std::forward<Args>(args)...);
			binds.push_back(std::move(bind));
			return *static_cast<T*>(binds.back().get());
		}

	private:
		std::vector<std::shared_ptr<IBindable>> binds;
	};
}
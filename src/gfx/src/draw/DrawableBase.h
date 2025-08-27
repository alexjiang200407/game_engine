#pragma once
#include "bindings/Bindable.h"
#include "bindings/IndexBuffer.h"
#include "draw/Drawable.h"
#include <memory>
#include <vector>

namespace gfx
{
	template <class T>
	class DrawableBase : public Drawable
	{
		friend T;

	private:
		DrawableBase(DX11Graphics& gfx)
		{
			if (refCount.fetch_sub(1, std::memory_order_acq_rel) == 0)
			{
				T::StaticBindingsConstructor(gfx, *this);
			}
		}

		~DrawableBase()
		{
			if (refCount.fetch_sub(1, std::memory_order_acq_rel) == 1)
			{
				staticBinds.clear();
			}
		}

		void
		Update(float) noexcept override {};

		template <typename T, typename... Args>
		T&
		AddStaticBind(Args&&... args)
		{
			using U = std::decay_t<T>;

			auto bind = std::make_unique<U>(std::forward<Args>(args)...);
			staticBinds.push_back(std::move(bind));
			return *static_cast<U*>(staticBinds.back().get());
		}

		const std::vector<std::unique_ptr<Bindable>>&
		GetStaticBinds() const noexcept override
		{
			return staticBinds;
		}

	private:
		static inline std::atomic<bool>                      initialized{ false };
		static inline std::atomic<size_t>                    refCount{ 0 };
		static inline std::vector<std::unique_ptr<Bindable>> staticBinds;
	};
}

#define DECLARE_STATIC_BINDINGS_CONSTRUCTOR(Type) \
	static void StaticBindingsConstructor(DX11Graphics& gfx, DrawableBase<Type>& boxBase)
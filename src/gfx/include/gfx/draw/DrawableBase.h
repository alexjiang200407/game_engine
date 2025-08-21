#pragma once
#include "gfx/bindings/Bindable.h"
#include "gfx/bindings/IndexBuffer.h"
#include "gfx/draw/Drawable.h"
#include <memory>
#include <vector>

namespace gfx
{
	template <class T>
	class DrawableBase : public Drawable
	{
	protected:
		template <typename T, typename... Args>
		T&
		AddStaticBind(Args&&... args)
		{
			using U = std::decay_t<T>;

			auto bind = std::make_unique<U>(std::forward<Args>(args)...);
			staticBinds.push_back(std::move(bind));
			return *static_cast<U*>(staticBinds.back().get());
		}

		static bool
		AcquireInitialization()
		{
			return !initialized.exchange(true, std::memory_order_acquire);
		}

	public:
		DrawableBase() { refCount.fetch_add(1); }

		~DrawableBase()
		{
			if (refCount.fetch_sub(1, std::memory_order_acq_rel) == 1)
			{
				staticBinds.clear();
			}
		}

	private:
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

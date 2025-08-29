#pragma once
#include <util/events/Consumer.h>

namespace util
{
	template <typename Event>
	class Consumer;

	template <typename Event>
	class Producer
	{
	public:
		virtual ~Producer() noexcept = default;

		void
		RegisterConsumer(std::shared_ptr<Consumer<Event>> c)
		{
			consumers.push_back(c);
		}

		template <typename T, typename... Args>
		std::shared_ptr<T>
		RegisterConsumer(Args&&... args)
		{
			auto shared = std::make_shared<T>(std::forward<Args>(args)...);
			RegisterConsumer(shared);
			return shared;
		}

	protected:
		void
		Produce(const Event& evt)
		{
			for (auto it = consumers.begin(); it != consumers.end();)
			{
				if (auto c = it->lock())
				{
					c->Consume(evt, *this);
					++it;
				}
				else
				{
					it = consumers.erase(it);
				}
			}
		}

	private:
		std::vector<std::weak_ptr<Consumer<Event>>> consumers;
	};
}
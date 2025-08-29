#pragma once
#include <util/events/Producer.h>

namespace util
{
	template <typename Event>
	class Producer;

	template <typename Event>
	class Consumer
	{
		friend class Producer<Event>;
	private:
		virtual void
		Consume(const Event& event, const Producer<Event>& producer) = 0;
	};
}

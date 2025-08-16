#pragma once
#include <cassert>
#include <cstddef>
#include <memory>
#include <type_traits>

namespace util
{
	template <typename T, std::size_t N>
	class RingQueue
	{
		static_assert(N != 0, "N cannot be equal to zero!");

	public:
		RingQueue() noexcept : head(0), tail(0), count(0) {}

		~RingQueue() noexcept { clear(); }

		// Non-copyable
		RingQueue(const RingQueue&) = delete;
		RingQueue&
		operator=(const RingQueue&) = delete;

		// Push by copy, fails if full
		bool
		push(const T& value) noexcept(std::is_nothrow_copy_constructible_v<T>)
		{
			if (full())
				return false;
			new (&data[tail]) T(value);  // in-place copy
			tail = nextIndex(tail);
			++count;
			return true;
		}

		// Push by move, fails if full
		bool
		push(T&& value) noexcept(std::is_nothrow_move_constructible_v<T>)
		{
			if (full())
				return false;
			new (&data[tail]) T(std::move(value));  // in-place move
			tail = nextIndex(tail);
			++count;
			return true;
		}

		// Emplace in-place, fails if full
		template <typename... Args>
		bool
		emplace(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
		{
			if (full())
				return false;
			new (&data[tail]) T{ std::forward<Args>(args)... };
			tail = nextIndex(tail);
			++count;
			return true;
		}

		// Pop front element
		void
		pop() noexcept(std::is_nothrow_destructible_v<T>)
		{
			data[head].~T();  // manual destruction
			head = nextIndex(head);
			--count;
		}

		// Access front element (assert if empty)
		T&
		front() noexcept
		{
			assert(!empty() && "RingBuffer::front() called on empty buffer");
			return data[head];
		}

		const T&
		front() const noexcept
		{
			assert(!empty() && "RingBuffer::front() called on empty buffer");
			return data[head];
		}

		// Access back element (assert if empty)
		T&
		back() noexcept
		{
			assert(!empty() && "RingBuffer::back() called on empty buffer");
			return data[prevIndex(tail)];
		}

		const T&
		back() const noexcept
		{
			assert(!empty() && "RingBuffer::back() called on empty buffer");
			return data[prevIndex(tail)];
		}

		void
		clear() noexcept(std::is_nothrow_destructible_v<T>)
		{
			while (!empty())
			{
				data[head].~T();
				head = nextIndex(head);
				--count;
			}
			head = tail = 0;
		}

		bool
		empty() const noexcept
		{
			return count == 0;
		}
		bool
		full() const noexcept
		{
			return count == N;
		}
		std::size_t
		size() const noexcept
		{
			return count;
		}
		static constexpr std::size_t
		capacity() noexcept
		{
			return N;
		}

	private:
		std::size_t
		nextIndex(std::size_t idx) const noexcept
		{
			return (idx + 1) % N;
		}
		std::size_t
		prevIndex(std::size_t idx) const noexcept
		{
			return (idx + N - 1) % N;
		}

		alignas(T) unsigned char rawData[sizeof(T) * N];
		T* const data = reinterpret_cast<T*>(rawData);

		std::size_t head;
		std::size_t tail;
		std::size_t count;
	};
}

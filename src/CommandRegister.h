#pragma once
#include <shared_mutex>

namespace ImGui
{
	class CommandRegister
	{
	public:
		static CommandRegister&
		Instance() noexcept;

		void
		Unregister(const std::string& cmdName) noexcept;

		void
		Execute();

		void
		Process();

		void
		DrawControlWindow() noexcept;

	private:
		CommandRegister();
		~CommandRegister()                  = default;
		CommandRegister(const CommandRegister&) = delete;
		CommandRegister&
		operator=(const CommandRegister&) = delete;

		std::string buf;
		std::string lastError;
		std::function<void()> pendingCmd;

	private:
		struct Node
		{
			std::unordered_map<char, std::unique_ptr<Node>> children; 
			std::function<void(std::istream&)>              handler;
			bool                                            hasHandler = false;
		};
		
		using NodePtr = std::unique_ptr<Node>;
		
		Node*
		FindNodeCreate(const std::string& key);
		
		Node*
		FindNode(const std::string& key) const;

		void
		PruneEmptyNodes(const std::string& key) noexcept;		
		
		NodePtr root;

		mutable std::shared_mutex mutex;

	private:
		template <typename T>
		static T
		ParseArg(std::istream& is)
		{
			if constexpr (std::is_same_v<T, std::string>)
			{
				std::string s;
				if (!(is >> s))
					throw std::runtime_error("Missing string argument");
				return s;
			}
			else if constexpr (std::is_same_v<T, bool>)
			{
				// Try reading bool directly
				bool b;
				if (is >> std::boolalpha >> b)
					return b;

				// If direct bool read fails, try int 0/1
				is.clear();
				int tmp;
				if (!(is >> tmp))
					throw std::runtime_error("Missing bool argument");
				if (tmp == 0)
					return false;
				if (tmp == 1)
					return true;

				throw std::invalid_argument("Invalid bool value");
			}
			else if constexpr (std::is_integral_v<T>)
			{
				T value;
				if constexpr (std::is_signed_v<T>)
				{
					int tmp;
					if (!(is >> tmp))
						throw std::runtime_error("Missing signed integer argument");
					
					if (tmp < std::numeric_limits<T>::min() || tmp > std::numeric_limits<T>::max())
						throw std::out_of_range("Signed integer out of range");
					
					value = static_cast<T>(tmp);
				}
				else
				{
					unsigned int tmp;
					if (!(is >> tmp))
						throw std::runtime_error("Missing unsigned integer argument");

					if (tmp > std::numeric_limits<T>::max())
						throw std::out_of_range("Unsigned integer out of range");
					value = static_cast<T>(tmp);
				}

				return value;
			}
			else if constexpr (std::is_floating_point_v<T>)
			{
				float tmp;
				if (!(is >> tmp))
					throw std::runtime_error("Missing float argument");
				try
				{
					return static_cast<T>(std::stod(tmp));
				}
				catch (...)
				{
					throw std::invalid_argument("Invalid float: " + tmp);
				}
			}
			else
				static_assert(!sizeof(T*), "Unsupported argument type");
		}

		template <typename Tuple, std::size_t... I>
		static auto
		ParseTupleImpl(std::istream& is, std::index_sequence<I...>)
		{
			std::tuple<std::tuple_element_t<I, Tuple>...> result;
			((std::get<I>(result) = ParseArg<std::tuple_element_t<I, Tuple>>(is)), ...);
			return result;
		}

		template <typename Tuple>
		static auto
		ParseTuple(std::istream& is)
		{
			constexpr size_t N = std::tuple_size<Tuple>::value;
			return ParseTupleImpl<Tuple>(is, std::make_index_sequence<N>{});
		}

	private:
		template <typename T>
		struct function_traits : function_traits<decltype(&T::operator())>
		{};

		template <typename C, typename R, typename... Args>
		struct function_traits<R (C::*)(Args...) const>
		{
			using args_tuple = std::tuple<Args...>;
		};

		template <typename F>
		static std::function<void(std::istream&)>
		MakeWrapper(F&& f)
		{
			using traits           = function_traits<std::decay_t<F>>;
			using args_tuple       = typename traits::args_tuple;

			return [fn = std::forward<F>(f)](std::istream& is) {
				auto tup = ParseTuple<args_tuple>(is);

				std::string leftover;
				if (is >> leftover)
					throw std::runtime_error("Too many arguments for command");

				std::apply(fn, tup);
			};
		}

	public:
		template <typename F>
		void
		Register(const std::string& cmdName, F&& f)
		{
			auto             wrapper = MakeWrapper(std::forward<F>(f));
			std::unique_lock lock(mutex);
			Node*            node = FindNodeCreate(cmdName);
			node->handler         = std::move(wrapper);
			node->hasHandler      = true;
		}
	};
}
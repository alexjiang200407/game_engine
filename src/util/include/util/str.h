#pragma once
namespace util
{
	template <typename T>
	bool
	parseStr(const char* str, T& buf)
	{
		std::istringstream iss(str);
		iss >> buf;

		bool success = iss.eof() && !iss.fail();

		return success;
	}

	[[nodiscard]]
	std::wstring
	stow(const std::string_view& value);

	[[nodiscard]]
	std::string
	wtos(const std::wstring_view& value);

	class cstring_view
	{
	public:
		using value_type     = char;
		using const_pointer  = const char*;
		using const_iterator = const char*;
		using size_type      = std::size_t;

		constexpr cstring_view() noexcept : ptr_(""), len_(0) {}
		constexpr cstring_view(const char* s, size_t len) : ptr_(s), len_(len) {}
		constexpr cstring_view(const std::string& s) : ptr_(s.c_str()), len_(s.size()) {}
		template <size_t N>
		constexpr cstring_view(const char (&s)[N]) noexcept : ptr_(s), len_(N - 1)
		{
			static_assert(N > 0 && s[N - 1] == '\0', "String literal must be null-terminated");
		}

		cstring_view(const char* s) : ptr_(s), len_(std::strlen(s)) {}

		constexpr const char*
		data() const noexcept
		{
			return ptr_;
		}
		constexpr const char*
		c_str() const noexcept
		{
			return ptr_;
		}
		constexpr size_type
		size() const noexcept
		{
			return len_;
		}
		constexpr bool
		empty() const noexcept
		{
			return len_ == 0;
		}

		constexpr const_iterator
		begin() const noexcept
		{
			return ptr_;
		}
		constexpr const_iterator
		end() const noexcept
		{
			return ptr_ + len_;
		}

		constexpr char
		operator[](size_type i) const
		{
			return i < len_ ? ptr_[i] : throw std::out_of_range("cstring_view");
		}

		constexpr
		operator std::string_view() const noexcept
		{
			return std::string_view(ptr_, len_);
		}

	private:
		const char* ptr_;
		size_type   len_;
	};

	inline std::ostream&
	operator<<(std::ostream& os, cstring_view sv)
	{
		return os.write(sv.data(), static_cast<std::streamsize>(sv.size()));
	}
}

constexpr util::cstring_view
operator"" csv(const char* str, std::size_t len) noexcept
{
	return util::cstring_view(str, len);
}

#pragma once

#define LEAF_ELEMENT_TYPES \
	X(Float)               \
	X(Float2)              \
	X(Float3)              \
	X(Float4)              \
	X(Matrix)              \
	X(Bool)

namespace gfx::dcb
{
	namespace dx = DirectX;

	enum Type
	{
#define X(el) el,
		LEAF_ELEMENT_TYPES
#undef X
			Struct,
		Array,
		Empty,
	};

	template <Type type>
	struct Map
	{
		static constexpr bool valid = false;
	};
	template <>
	struct Map<Float>
	{
		using SysType                         = float;
		static constexpr size_t      hlslSize = sizeof(SysType);
		static constexpr const char* code     = "F1";
		static constexpr bool        valid    = true;
	};
	template <>
	struct Map<Float2>
	{
		using SysType                         = dx::XMFLOAT2;
		static constexpr size_t      hlslSize = sizeof(SysType);
		static constexpr const char* code     = "F2";
		static constexpr bool        valid    = true;
	};
	template <>
	struct Map<Float3>
	{
		using SysType                         = dx::XMFLOAT3;
		static constexpr size_t      hlslSize = sizeof(SysType);
		static constexpr const char* code     = "F3";
		static constexpr bool        valid    = true;
	};
	template <>
	struct Map<Float4>
	{
		using SysType                         = dx::XMFLOAT4;
		static constexpr size_t      hlslSize = sizeof(SysType);
		static constexpr const char* code     = "F4";
		static constexpr bool        valid    = true;
	};
	template <>
	struct Map<Matrix>
	{
		using SysType                         = dx::XMFLOAT4X4;
		static constexpr size_t      hlslSize = sizeof(SysType);
		static constexpr const char* code     = "M4";
		static constexpr bool        valid    = true;
	};
	template <>
	struct Map<Bool>
	{
		using SysType                         = bool;
		static constexpr size_t      hlslSize = 4u;
		static constexpr const char* code     = "BL";
		static constexpr bool        valid    = true;
	};

#define X(el) static_assert(Map<el>::valid, "Missing map implementation for " #el);
	LEAF_ELEMENT_TYPES
#undef X

	template <typename T>
	struct ReverseMap
	{
		static constexpr bool valid = false;
	};
#define X(el)                                    \
	template <>                                  \
	struct ReverseMap<typename Map<el>::SysType> \
	{                                            \
		static constexpr Type type  = el;        \
		static constexpr bool valid = true;      \
	};
	LEAF_ELEMENT_TYPES
#undef X

	class LayoutElement
	{
	private:
		struct ExtraDataBase
		{
			virtual ~ExtraDataBase() = default;
		};
		friend class RawLayout;
		friend struct ExtraData;

	public:
		std::string
		GetSignature() const;

		bool
		Exists() const noexcept;

		std::pair<size_t, const LayoutElement*>
		CalculateIndexingOffset(size_t offset, size_t index) const;

		LayoutElement&
		operator[](const std::string& key);

		const LayoutElement&
		operator[](const std::string& key) const;

		LayoutElement&
		T();

		const LayoutElement&
		T() const;

		size_t
		GetOffsetBegin() const;
		size_t
		GetOffsetEnd() const;

		size_t
		GetSizeInBytes() const;

		LayoutElement&
		Add(Type addedType, std::string name);
		template <Type typeAdded>
		LayoutElement&
		Add(std::string key)
		{
			return Add(typeAdded, std::move(key));
		}

		LayoutElement&
		Set(Type addedType, size_t size);

		template <Type typeAdded>
		LayoutElement&
		Set(size_t size)
		{
			return Set(typeAdded, size);
		}

		template <typename T>
		size_t
		Resolve() const
		{
			switch (type)
			{
#define X(el)                                          \
	case el:                                           \
		assert(typeid(Map<el>::SysType) == typeid(T)); \
		return *offset;
				LEAF_ELEMENT_TYPES
#undef X
			default:
				assert("Tried to resolve non-leaf element" && false);
				return 0u;
			}
		}

	private:
		LayoutElement() noexcept = default;
		LayoutElement(Type typeIn);
		size_t
		Finalize(size_t offsetIn);
		std::string
		GetSignatureForStruct() const;
		std::string
		GetSignatureForArray() const;
		size_t
		FinalizeForStruct(size_t offsetIn);
		size_t
		FinalizeForArray(size_t offsetIn);
		static LayoutElement&
		GetEmptyElement() noexcept
		{
			static LayoutElement empty{};
			return empty;
		}
		static size_t
		AdvanceToBoundary(size_t offset) noexcept;
		static bool
		CrossesBoundary(size_t offset, size_t size) noexcept;
		static size_t
		AdvanceIfCrossesBoundary(size_t offset, size_t size) noexcept;
		static bool
		ValidateSymbolName(const std::string& name) noexcept;

	private:
		std::optional<size_t>          offset;
		Type                           type = Empty;
		std::unique_ptr<ExtraDataBase> pExtraData;
	};

	class Layout
	{
		friend class LayoutCodex;
		friend class Buffer;

	public:
		size_t
		GetSizeInBytes() const noexcept;
		std::string
		GetSignature() const;

	protected:
		Layout(std::shared_ptr<LayoutElement> pRoot) noexcept;
		std::shared_ptr<LayoutElement> pRoot;
	};

	class RawLayout : public Layout
	{
		friend class LayoutCodex;

	public:
		RawLayout() noexcept;

		LayoutElement&
		operator[](const std::string& key);

		template <Type type>
		LayoutElement&
		Add(const std::string& key)
		{
			return pRoot->Add<type>(key);
		}

	private:
		void
		ClearRoot() noexcept;
		std::shared_ptr<LayoutElement>
		DeliverRoot() noexcept;
	};

	class CookedLayout : public Layout
	{
		friend class LayoutCodex;
		friend class Buffer;

	public:
		const LayoutElement&
		operator[](const std::string& key) const;
		std::shared_ptr<LayoutElement>
		ShareRoot() const noexcept;

	private:
		CookedLayout(std::shared_ptr<LayoutElement> pRoot) noexcept;
		std::shared_ptr<LayoutElement>
		RelinquishRoot() const noexcept;
	};

	class ConstElementRef
	{
		friend class Buffer;
		friend class ElementRef;

	public:
		class Ptr
		{
			friend ConstElementRef;

		public:
			template <typename T>
			operator const T*() const
			{
				static_assert(
					ReverseMap<std::remove_const_t<T>>::valid,
					"Unsupported SysType used in pointer conversion");
				return &static_cast<const T&>(*ref);
			}

		private:
			Ptr(const ConstElementRef* ref) noexcept;
			const ConstElementRef* ref;
		};

	public:
		bool
		Exists() const noexcept;

		ConstElementRef
		operator[](const std::string& key) const;

		ConstElementRef
		operator[](size_t index) const;

		Ptr
		operator&() const;

		template <typename T>
		operator const T&() const
		{
			static_assert(
				ReverseMap<std::remove_const_t<T>>::valid,
				"Unsupported SysType used in conversion");
			return *reinterpret_cast<const T*>(pBytes + offset + pLayout->Resolve<T>());
		}

	private:
		ConstElementRef(const LayoutElement* pLayout, const char* pBytes, size_t offset) noexcept;

		size_t               offset;
		const LayoutElement* pLayout;
		const char*          pBytes;
	};

	class ElementRef
	{
		friend class Buffer;

	public:
		class Ptr
		{
			friend ElementRef;

		public:
			template <typename T>
			operator T*() const
			{
				static_assert(
					ReverseMap<std::remove_const_t<T>>::valid,
					"Unsupported SysType used in pointer conversion");
				return &static_cast<T&>(*ref);
			}

		private:
			Ptr(ElementRef* ref) noexcept;
			ElementRef* ref;
		};

	public:
		operator ConstElementRef() const noexcept;
		bool
		Exists() const noexcept;

		ElementRef
		operator[](const std::string& key) const;

		ElementRef
		operator[](size_t index) const;

		template <typename S>
		bool
		SetIfExists(const S& val)
		{
			if (Exists())
			{
				*this = val;
				return true;
			}
			return false;
		}
		Ptr
		operator&() const;

		template <typename T>
		operator T&() const
		{
			static_assert(
				ReverseMap<std::remove_const_t<T>>::valid,
				"Unsupported SysType used in conversion");
			return *reinterpret_cast<T*>(pBytes + offset + pLayout->Resolve<T>());
		}

		template <typename T>
		T&
		operator=(const T& rhs) const
		{
			static_assert(
				ReverseMap<std::remove_const_t<T>>::valid,
				"Unsupported SysType used in assignment");
			return static_cast<T&>(*this) = rhs;
		}

	private:
		ElementRef(const LayoutElement* pLayout, char* pBytes, size_t offset) noexcept;
		size_t               offset;
		const LayoutElement* pLayout;
		char*                pBytes;
	};

	class Buffer
	{
	public:
		Buffer(RawLayout&& lay);
		Buffer(const CookedLayout& lay);
		Buffer(CookedLayout&& lay);
		Buffer(const Buffer&) noexcept;

		Buffer(Buffer&&) noexcept;

		ElementRef
		operator[](const std::string& key);

		ConstElementRef
		operator[](const std::string& key) const;

		const char*
		GetData() const noexcept;

		size_t
		GetSizeInBytes() const noexcept;

		const LayoutElement&
		GetRootLayoutElement() const noexcept;

		void
		CopyFrom(const Buffer&);

		std::shared_ptr<LayoutElement>
		ShareLayoutRoot() const noexcept;

	private:
		std::shared_ptr<LayoutElement> pLayoutRoot;
		std::vector<char>              bytes;
	};
}

#ifndef DCB_IMPL_SOURCE
#	undef LEAF_ELEMENT_TYPES
#endif

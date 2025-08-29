#pragma once
#include <type_traits>
#include <vector>

namespace gfx::geom
{
	struct RGBAColor
	{
		unsigned char r;
		unsigned char g;
		unsigned char b;
		unsigned char a;
	};

	class VertexLayout
	{
	public:
		enum class ElementType
		{
			Position2D,
			Position3D,
			Texture2D,
			Normal,
			Float3Color,
			Float4Color,
			RGBAColor,
			TotalCount
		};
		template <ElementType>
		struct Map;

		template <>
		struct Map<ElementType::Position2D>
		{
			using SysType                           = DirectX::XMFLOAT2;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32_FLOAT;
			static constexpr const char* semantic   = "Position";
		};
		template <>
		struct Map<ElementType::Position3D>
		{
			using SysType                           = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic   = "Position";
		};
		template <>
		struct Map<ElementType::Texture2D>
		{
			using SysType                           = DirectX::XMFLOAT2;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32_FLOAT;
			static constexpr const char* semantic   = "Texcoord";
		};
		template <>
		struct Map<ElementType::Normal>
		{
			using SysType                           = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic   = "Normal";
		};
		template <>
		struct Map<ElementType::Float3Color>
		{
			using SysType                           = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic   = "Color";
		};
		template <>
		struct Map<ElementType::Float4Color>
		{
			using SysType                           = DirectX::XMFLOAT4;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
			static constexpr const char* semantic   = "Color";
		};
		template <>
		struct Map<ElementType::RGBAColor>
		{
			using SysType                           = RGBAColor;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
			static constexpr const char* semantic   = "Color";
		};

		class Element
		{
		public:
			Element(ElementType type, size_t offset) : type(type), offset(offset) {}

			size_t
			GetOffsetAfter() const noexcept
			{
				return offset + Size();
			}

			size_t
			GetOffset() const
			{
				return offset;
			}

			size_t
			Size() const noexcept
			{
				return SizeOf(type);
			}

			static constexpr size_t
			SizeOf(ElementType type) noexcept
			{
				using namespace DirectX;
				switch (type)
				{
				case ElementType::Position2D:
					return sizeof(Map<ElementType::Position2D>::SysType);
				case ElementType::Position3D:
					return sizeof(Map<ElementType::Position3D>::SysType);
				case ElementType::Texture2D:
					return sizeof(Map<ElementType::Texture2D>::SysType);
				case ElementType::Normal:
					return sizeof(Map<ElementType::Normal>::SysType);
				case ElementType::Float3Color:
					return sizeof(Map<ElementType::Float3Color>::SysType);
				case ElementType::Float4Color:
					return sizeof(Map<ElementType::Float4Color>::SysType);
				case ElementType::RGBAColor:
					return sizeof(Map<ElementType::RGBAColor>::SysType);
				default:
					assert("Invalid element type" && false);
				}
				return 0u;
			}
			ElementType
			GetType() const noexcept
			{
				return type;
			}

			D3D11_INPUT_ELEMENT_DESC
			GetDesc() const noexcept
			{
				using ElementType = VertexLayout::ElementType;
				switch (type)
				{
				case ElementType::Position2D:
					return GenerateDesc<ElementType::Position2D>(GetOffset());
				case ElementType::Position3D:
					return GenerateDesc<ElementType::Position3D>(GetOffset());
				case ElementType::Texture2D:
					return GenerateDesc<ElementType::Texture2D>(GetOffset());
				case ElementType::Normal:
					return GenerateDesc<ElementType::Normal>(GetOffset());
				case ElementType::Float3Color:
					return GenerateDesc<ElementType::Float3Color>(GetOffset());
				case ElementType::Float4Color:
					return GenerateDesc<ElementType::Float4Color>(GetOffset());
				case ElementType::RGBAColor:
					return GenerateDesc<ElementType::RGBAColor>(GetOffset());
				}
				assert("Invalid element type" && false);
				return { "INVALID", 0, DXGI_FORMAT_UNKNOWN, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
			}

		private:
			template <ElementType type>
			static constexpr D3D11_INPUT_ELEMENT_DESC
			GenerateDesc(size_t offset) noexcept
			{
				return { Map<type>::semantic,         0, Map<type>::dxgiFormat, 0, (UINT)offset,
					     D3D11_INPUT_PER_VERTEX_DATA, 0 };
			}

		private:
			ElementType type;
			size_t      offset;
		};

	public:
		template <ElementType Type>
		const Element&
		Resolve() const noexcept
		{
			for (auto& e : elements)
			{
				if (e.GetType() == Type)
				{
					return e;
				}
			}
			assert("Could not resolve element type" && false);
			return elements.front();
		}

		const Element&
		ResolveByIndex(size_t i) const noexcept
		{
			return elements[i];
		}

		VertexLayout&
		Append(ElementType type)
		{
			elements.emplace_back(type, Size());
			return *this;
		}

		size_t
		Size() const noexcept
		{
			return elements.empty() ? 0u : elements.back().GetOffsetAfter();
		}

		size_t
		GetElementCount() const noexcept
		{
			return elements.size();
		}

		std::vector<D3D11_INPUT_ELEMENT_DESC>
		GetD3DLayout() const
		{
			std::vector<D3D11_INPUT_ELEMENT_DESC> desc;
			desc.reserve(GetElementCount());
			for (const auto& e : elements)
			{
				desc.push_back(e.GetDesc());
			}
			return desc;
		}

	private:
		std::vector<Element> elements;
	};

	class VertexBuffer;

	class Vertex
	{
		friend class VertexBuffer;

	public:
		template <VertexLayout::ElementType Type>
		auto&
		Attr() noexcept
		{
			auto pAttribute = pData + layout.Resolve<Type>().GetOffset();
			return *reinterpret_cast<typename VertexLayout::Map<Type>::SysType*>(pAttribute);
		}

		template <typename T>
		void
		SetAttributeByIndex(size_t i, T&& val) noexcept
		{
			using namespace DirectX;
			const auto& element    = layout.ResolveByIndex(i);
			auto        pAttribute = pData + element.GetOffset();

			using Type = VertexLayout::ElementType;

			switch (element.GetType())
			{
			case Type::Position2D:
				SetAttribute<Type::Position2D>(pAttribute, std::forward<T>(val));
				break;
			case Type::Position3D:
				SetAttribute<Type::Position3D>(pAttribute, std::forward<T>(val));
				break;
			case Type::Texture2D:
				SetAttribute<Type::Texture2D>(pAttribute, std::forward<T>(val));
				break;
			case Type::Normal:
				SetAttribute<Type::Normal>(pAttribute, std::forward<T>(val));
				break;
			case Type::Float3Color:
				SetAttribute<Type::Float3Color>(pAttribute, std::forward<T>(val));
				break;
			case Type::Float4Color:
				SetAttribute<Type::Float4Color>(pAttribute, std::forward<T>(val));
				break;
			case Type::RGBAColor:
				SetAttribute<Type::RGBAColor>(pAttribute, std::forward<T>(val));
				break;
			default:
				assert("Bad element type" && false);
			}
		}

	protected:
		Vertex(char* pData, const VertexLayout& layout) noexcept : pData(pData), layout(layout)
		{
			assert(pData != nullptr);
		}

	private:
		template <typename First, typename... Rest>
		void
		SetAttributeByIndex(size_t i, First&& first, Rest&&... rest) noexcept
		{
			SetAttributeByIndex(i, std::forward<First>(first));
			SetAttributeByIndex(i + 1, std::forward<Rest>(rest)...);
		}

		template <VertexLayout::ElementType DestLayoutType, typename SrcType>
		void
		SetAttribute(char* pAttribute, SrcType&& val) noexcept
		{
			using Dest = typename VertexLayout::Map<DestLayoutType>::SysType;
			if constexpr (std::is_assignable<Dest, SrcType>::value)
			{
				*reinterpret_cast<Dest*>(pAttribute) = val;
			}
			else
			{
				assert("Parameter attribute type mismatch" && false);
			}
		}

	private:
		char*               pData = nullptr;
		const VertexLayout& layout;
	};

	class ConstVertex
	{
	public:
		ConstVertex(const Vertex& v) noexcept : vertex(v) {}
		template <VertexLayout::ElementType Type>
		const auto&
		Attr() const noexcept
		{
			return const_cast<Vertex&>(vertex).Attr<Type>();
		}

	private:
		Vertex vertex;
	};

	class VertexBuffer
	{
	public:
		VertexBuffer(VertexLayout layout) noexcept : layout(std::move(layout)) {}

		const VertexLayout&
		GetLayout() const noexcept
		{
			return layout;
		}

		size_t
		Size() const noexcept
		{
			return buffer.size() / layout.Size();
		}

		size_t
		SizeBytes() const noexcept
		{
			return buffer.size();
		}

		template <typename... Args>
		void
		EmplaceBack(Args&&... args)
		{
			assert(
				sizeof...(args) == layout.GetElementCount() &&
				"Param count doesn't match number of vertex elements");
			buffer.resize(buffer.size() + layout.Size());
			Back().SetAttributeByIndex(0u, std::forward<Args>(args)...);
		}

		void
		Reserve(size_t capacity)
		{
			buffer.reserve(capacity * layout.Size());
		}

		Vertex
		Back() noexcept
		{
			assert(buffer.size() != 0u);
			return Vertex{ buffer.data() + buffer.size() - layout.Size(), layout };
		}

		Vertex
		Front() noexcept
		{
			assert(buffer.size() != 0u);
			return Vertex{ buffer.data(), layout };
		}

		Vertex
		operator[](size_t i) noexcept
		{
			assert(i < Size());
			return Vertex{ buffer.data() + layout.Size() * i, layout };
		}

		ConstVertex
		Back() const noexcept
		{
			return const_cast<VertexBuffer&>(*this).Back();
		}

		ConstVertex
		Front() const noexcept
		{
			return const_cast<VertexBuffer&>(*this).Front();
		}

		ConstVertex
		operator[](size_t i) const noexcept
		{
			return const_cast<VertexBuffer&>(*this)[i];
		}

		const char*
		GetData() const noexcept
		{
			return buffer.data();
		}

	private:
		std::vector<char> buffer;
		VertexLayout      layout;
	};
}

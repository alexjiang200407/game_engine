#include "geom/Vertex.h"

size_t
gfx::geom::VertexLayout::Element::GetOffsetAfter() const noexcept
{
	return offset + Size();
}

size_t
gfx::geom::VertexLayout::Element::GetOffset() const
{
	return offset;
}

size_t
gfx::geom::VertexLayout::Element::Size() const noexcept
{
	return SizeOf(type);
}

constexpr size_t
gfx::geom::VertexLayout::Element::SizeOf(ElementType type) noexcept
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
	case ElementType::Tangent:
		return sizeof(Map<ElementType::Tangent>::SysType);
	case ElementType::BiTangent:
		return sizeof(Map<ElementType::BiTangent>::SysType);
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

gfx::geom::VertexLayout::ElementType
gfx::geom::VertexLayout::Element::GetType() const noexcept
{
	return type;
}

D3D11_INPUT_ELEMENT_DESC
gfx::geom::VertexLayout::Element::GetDesc() const noexcept
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
	case ElementType::Tangent:
		return GenerateDesc<ElementType::Tangent>(GetOffset());
	case ElementType::BiTangent:
		return GenerateDesc<ElementType::BiTangent>(GetOffset());
	}
	assert("Invalid element type" && false);
	return { "INVALID", 0, DXGI_FORMAT_UNKNOWN, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
}

const char*
gfx::geom::VertexLayout::Element::GetCode() const noexcept
{
	using ElementType = VertexLayout::ElementType;

	switch (type)
	{
	case ElementType::Position2D:
		return Map<ElementType::Position2D>::code;
	case ElementType::Position3D:
		return Map<ElementType::Position3D>::code;
	case ElementType::Texture2D:
		return Map<ElementType::Texture2D>::code;
	case ElementType::Normal:
		return Map<ElementType::Normal>::code;
	case ElementType::Float3Color:
		return Map<ElementType::Float3Color>::code;
	case ElementType::Float4Color:
		return Map<ElementType::Float4Color>::code;
	case ElementType::RGBAColor:
		return Map<ElementType::RGBAColor>::code;
	case ElementType::Tangent:
		return Map<ElementType::Tangent>::code;
	case ElementType::BiTangent:
		return Map<ElementType::BiTangent>::code;
	}
	assert("Invalid element type" && false);
	return "Invalid";
}

const gfx::geom::VertexLayout::Element&
gfx::geom::VertexLayout::ResolveByIndex(size_t i) const noexcept
{
	return elements[i];
}

gfx::geom::VertexLayout&
gfx::geom::VertexLayout::Append(ElementType type)
{
	elements.emplace_back(type, Size());
	return *this;
}

size_t
gfx::geom::VertexLayout::Size() const noexcept
{
	return elements.empty() ? 0u : elements.back().GetOffsetAfter();
}

size_t
gfx::geom::VertexLayout::GetElementCount() const noexcept
{
	return elements.size();
}

std::vector<D3D11_INPUT_ELEMENT_DESC>
gfx::geom::VertexLayout::GetD3DLayout() const
{
	std::vector<D3D11_INPUT_ELEMENT_DESC> desc;
	desc.reserve(GetElementCount());
	for (const auto& e : elements)
	{
		desc.push_back(e.GetDesc());
	}
	return desc;
}

std::string
gfx::geom::VertexLayout::GetCode() const
{
	std::string code;
	for (const auto& e : elements)
	{
		code += "#"s;
		code += e.GetCode();
	}
	return code;
}

gfx::geom::ConstVertex::ConstVertex(const Vertex& v) noexcept : vertex(v) {}

gfx::geom::VertexBuffer::VertexBuffer(VertexLayout layout) noexcept : layout(std::move(layout)) {}

const gfx::geom::VertexLayout&
gfx::geom::VertexBuffer::GetLayout() const noexcept
{
	return layout;
}

size_t
gfx::geom::VertexBuffer::Size() const noexcept
{
	return buffer.size() / layout.Size();
}

size_t
gfx::geom::VertexBuffer::SizeBytes() const noexcept
{
	return buffer.size();
}

void
gfx::geom::VertexBuffer::Reserve(size_t capacity)
{
	buffer.reserve(capacity * layout.Size());
}

gfx::geom::Vertex
gfx::geom::VertexBuffer::Back() noexcept
{
	assert(buffer.size() != 0u);
	return Vertex{ buffer.data() + buffer.size() - layout.Size(), layout };
}

gfx::geom::Vertex
gfx::geom::VertexBuffer::Front() noexcept
{
	assert(buffer.size() != 0u);
	return Vertex{ buffer.data(), layout };
}

gfx::geom::Vertex
gfx::geom::VertexBuffer::operator[](size_t i) noexcept
{
	assert(i < Size());
	return Vertex{ buffer.data() + layout.Size() * i, layout };
}

gfx::geom::ConstVertex
gfx::geom::VertexBuffer::Back() const noexcept
{
	return const_cast<VertexBuffer&>(*this).Back();
}

gfx::geom::ConstVertex
gfx::geom::VertexBuffer::Front() const noexcept
{
	return const_cast<VertexBuffer&>(*this).Front();
}

gfx::geom::ConstVertex
gfx::geom::VertexBuffer::operator[](size_t i) const noexcept
{
	return const_cast<VertexBuffer&>(*this)[i];
}

const char*
gfx::geom::VertexBuffer::GetData() const noexcept
{
	return buffer.data();
}

gfx::geom::Vertex::Vertex(char* pData, const VertexLayout& layout) noexcept :
	pData(pData), layout(layout)
{
	assert(pData != nullptr);
}

std::ostream&
gfx::geom::operator<<(std::ostream& os, const VertexLayout& layout)
{
	return os << layout.GetCode();
}
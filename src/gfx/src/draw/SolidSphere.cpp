#include "draw/SolidSphere.h"
#include "bindings/Bindings.h"
#include "geom/Geometry.h"

using namespace std::literals;

gfx::SolidSphere::SolidSphere(DX11Graphics& gfx, float radius)
{
	namespace dx      = DirectX;
	using ElementType = geom::VertexLayout::ElementType;

	struct Vertex
	{
		dx::XMFLOAT3 pos;
	};

	auto model = geom::Sphere::Make<Vertex>();
	model.Transform(dx::XMMatrixScaling(radius, radius, radius));
	AddBind<VertexBuffer>({ "SolidSphere"sv, ""sv }, gfx, model.vertices, "SolidSphere"sv);
	AddBind<IndexBuffer>({ "SolidSphere"sv, ""sv }, gfx, model.indices, "SolidSphere"sv);
	AddUniqueBind<TransformCBuffer>(gfx, *this);

	static constexpr const auto* vertexShader = L"shaders/vs_solid.cso";
	static constexpr const auto* pixelShader  = L"shaders/ps_solid.cso";

	auto& pvs   = AddBind<VertexShader>({ vertexShader }, gfx, vertexShader);
	auto  pvsbc = pvs.GetBytecode();

	AddBind<PixelShader>({ pixelShader }, gfx, pixelShader);

	struct PSColorConstant
	{
		dx::XMFLOAT3 color = { 1.0f, 1.0f, 1.0f };
		float        padding;
	} colorConst;
	AddBind<PixelConstantBuffer<PSColorConstant>>({ 0u }, gfx, colorConst);

	geom::VertexBuffer vbuf(std::move(geom::VertexLayout{}.Append(ElementType::Position3D)));

	auto& layout = vbuf.GetLayout();
	AddBind<InputLayout>({ layout }, gfx, layout, pvsbc);

	AddBind<Topology>(
		{ static_cast<int>(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) },
		gfx,
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void
gfx::SolidSphere::Update(float) noexcept
{}

void
gfx::SolidSphere::SetPos(DirectX::XMFLOAT3 a_pos) noexcept
{
	this->pos = a_pos;
}

DirectX::XMMATRIX
gfx::SolidSphere::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
}
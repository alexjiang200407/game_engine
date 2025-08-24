#include "draw/SolidSphere.h"
#include "bindings/Bindings.h"
#include "geom/Geometry.h"

gfx::SolidSphere::SolidSphere(Graphics& gfx, float radius) : DrawableBase<SolidSphere>(gfx)
{
	namespace dx = DirectX;
	struct Vertex
	{
		dx::XMFLOAT3 pos;
	};

	auto model = geom::Sphere::Make<Vertex>();
	model.Transform(dx::XMMatrixScaling(radius, radius, radius));
	AddBind<VertexBuffer>(gfx, model.vertices);
	AddBind<IndexBuffer>(gfx, model.indices);
	AddBind<TransformCBuffer>(gfx, *this);
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

void
gfx::SolidSphere::StaticBindingsConstructor(
	Graphics&                  gfx,
	DrawableBase<SolidSphere>& solidSphereBase)
{
	namespace dx = DirectX;
	auto& pvs    = solidSphereBase.AddStaticBind<VertexShader>(gfx, L"shaders/vs_solid.cso");
	auto  pvsbc  = pvs.GetBytecode();

	solidSphereBase.AddStaticBind<PixelShader>(gfx, L"shaders/ps_solid.cso");

	struct PSColorConstant
	{
		dx::XMFLOAT3 color = { 1.0f, 1.0f, 1.0f };
		float        padding;
	} colorConst;
	solidSphereBase.AddStaticBind<PixelConstantBuffer<PSColorConstant>>(gfx, colorConst);

	const std::vector<D3D11_INPUT_ELEMENT_DESC> ied = {
		{ "Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	solidSphereBase.AddStaticBind<InputLayout>(gfx, ied, pvsbc);

	solidSphereBase.AddStaticBind<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

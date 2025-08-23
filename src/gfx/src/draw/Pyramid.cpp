#include "draw/Pyramid.h"
#include "bindings/Bindings.h"
#include "geom/Cone.h"

gfx::Pyramid::Pyramid(
	Graphics&                              gfx,
	std::mt19937&                          rng,
	std::uniform_real_distribution<float>& adist,
	std::uniform_real_distribution<float>& ddist,
	std::uniform_real_distribution<float>& odist,
	std::uniform_real_distribution<float>& rdist) :
	DrawableBase<Pyramid>(gfx), r(rdist(rng)), droll(ddist(rng)), dpitch(ddist(rng)),
	dyaw(ddist(rng)), dphi(odist(rng)), dtheta(odist(rng)), dchi(odist(rng)), chi(adist(rng)),
	theta(adist(rng)), phi(adist(rng))
{
	AddBind<TransformCBuffer>(gfx, *this);
}

void
gfx::Pyramid::Update(float dt) noexcept
{
	roll += droll * dt;
	pitch += dpitch * dt;
	yaw += dyaw * dt;
	theta += dtheta * dt;
	phi += dphi * dt;
	chi += dchi * dt;
}

DirectX::XMMATRIX
gfx::Pyramid::GetTransformXM() const noexcept
{
	namespace dx = DirectX;
	return dx::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
	       dx::XMMatrixTranslation(r, 0.0f, 0.0f) *
	       dx::XMMatrixRotationRollPitchYaw(theta, phi, chi);
}

void
gfx::Pyramid::StaticBindingsConstructor(Graphics& gfx, DrawableBase<Pyramid>& pyramidBase)
{
	namespace dx = DirectX;
	struct Vertex
	{
		dx::XMFLOAT3 pos;
		struct
		{
			unsigned char r;
			unsigned char g;
			unsigned char b;
			unsigned char a;
		} color;
	};
	auto model = geom::Cone::MakeTesselated<Vertex>(4);
	// set vertex colors for mesh
	model.vertices[0].color = { 255, 255, 0 };
	model.vertices[1].color = { 255, 255, 0 };
	model.vertices[2].color = { 255, 255, 0 };
	model.vertices[3].color = { 255, 255, 0 };
	model.vertices[4].color = { 255, 255, 80 };
	model.vertices[5].color = { 255, 10, 0 };
	// deform mesh linearly
	model.Transform(dx::XMMatrixScaling(1.0f, 1.0f, 0.7f));

	pyramidBase.AddStaticBind<VertexBuffer>(gfx, model.vertices);

	auto& pvs   = pyramidBase.AddStaticBind<VertexShader>(gfx, L"shaders/vs_color_blend.cso");
	auto  pvsbc = pvs.GetBytecode();

	pyramidBase.AddStaticBind<PixelShader>(gfx, L"shaders/ps_color_blend.cso");

	pyramidBase.AddStaticBind<IndexBuffer>(gfx, model.indices);

	const std::vector<D3D11_INPUT_ELEMENT_DESC> ied = {
		{ "Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "Color", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	pyramidBase.AddStaticBind<InputLayout>(gfx, ied, pvsbc);

	pyramidBase.AddStaticBind<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
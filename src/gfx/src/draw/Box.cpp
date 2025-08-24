#include "draw/Box.h"
#include "Graphics.h"
#include "bindings/Bindings.h"
#include "geom/Cube.h"

namespace dx = DirectX;

gfx::Box::Box(
	Graphics&                              gfx,
	std::mt19937&                          rng,
	std::uniform_real_distribution<float>& adist,
	std::uniform_real_distribution<float>& ddist,
	std::uniform_real_distribution<float>& odist,
	std::uniform_real_distribution<float>& rdist,
	std::uniform_real_distribution<float>& bdist) :
	DrawableBase<Box>(gfx), r(rdist(rng)), droll(ddist(rng)), dpitch(ddist(rng)), dyaw(ddist(rng)),
	dphi(odist(rng)), dtheta(odist(rng)), dchi(odist(rng)), chi(adist(rng)), theta(adist(rng)),
	phi(adist(rng))
{
	AddBind<TransformCBuffer>(gfx, *this);
	dx::XMStoreFloat3x3(&mt, dx::XMMatrixScaling(1.0f, 1.0f, bdist(rng)));
}

void
gfx::Box::StaticBindingsConstructor(Graphics& gfx, DrawableBase<Box>& boxBase)
{
	struct Vertex
	{
		dx::XMFLOAT3 pos;
		dx::XMFLOAT3 n;
	};
	auto model = geom::Cube::MakeIndependent<Vertex>();
	model.SetNormalsIndependentFlat();

	boxBase.AddStaticBind<VertexBuffer>(gfx, model.vertices);

	auto& pvs   = boxBase.AddStaticBind<VertexShader>(gfx, L"shaders/vs_phong.cso");
	auto  pvsbc = pvs.GetBytecode();

	boxBase.AddStaticBind<PixelShader>(gfx, L"shaders/ps_phong.cso");

	boxBase.AddStaticBind<IndexBuffer>(gfx, model.indices);

	const std::vector<D3D11_INPUT_ELEMENT_DESC> ied = {
		{ "Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "Normal",
		  0,
		  DXGI_FORMAT_R32G32B32_FLOAT,
		  0,
		  D3D11_APPEND_ALIGNED_ELEMENT,
		  D3D11_INPUT_PER_VERTEX_DATA,
		  0 },
	};
	boxBase.AddStaticBind<InputLayout>(gfx, ied, pvsbc);

	boxBase.AddStaticBind<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void
gfx::Box::Update(float dt) noexcept
{
	roll += droll * dt;
	pitch += dpitch * dt;
	yaw += dyaw * dt;
	theta += dtheta * dt;
	phi += dphi * dt;
	chi += dchi * dt;
}

DirectX::XMMATRIX
gfx::Box::GetTransformXM() const noexcept
{
	namespace dx = DirectX;
	return dx::XMLoadFloat3x3(&mt) * dx::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
	       dx::XMMatrixTranslation(r, 0.0f, 0.0f) *
	       dx::XMMatrixRotationRollPitchYaw(theta, phi, chi);
}
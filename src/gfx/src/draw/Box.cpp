#include "draw/Box.h"
#include "DX11Graphics.h"
#include "bindings/Bindings.h"
#include "geom/Cube.h"

namespace dx = DirectX;

gfx::Box::Box(
	DX11Graphics&                          gfx,
	std::mt19937&                          rng,
	std::uniform_real_distribution<float>& adist,
	std::uniform_real_distribution<float>& ddist,
	std::uniform_real_distribution<float>& odist,
	std::uniform_real_distribution<float>& rdist,
	std::uniform_real_distribution<float>& bdist,
	DirectX::XMFLOAT3                      material) :
	DrawableBase<Box>(gfx), r(rdist(rng)), droll(ddist(rng)), dpitch(ddist(rng)), dyaw(ddist(rng)),
	dphi(odist(rng)), dtheta(odist(rng)), dchi(odist(rng)), chi(adist(rng)), theta(adist(rng)),
	phi(adist(rng))
{
	AddBind<TransformCBuffer>(gfx, *this);

	struct PSMaterialConstant
	{
		dx::XMFLOAT3 color;
		float        specularIntensity = 0.6f;
		float        specularPower     = 30.0f;
		float        pad[3];
	} colorConst{};

	colorConst.color = material;
	AddBind<PixelConstantBuffer<PSMaterialConstant>>(gfx, colorConst, 1u);

	dx::XMStoreFloat3x3(&mt, dx::XMMatrixScaling(1.0f, 1.0f, bdist(rng)));
}

void
gfx::Box::StaticBindingsConstructor(DX11Graphics& gfx, DrawableBase<Box>& boxBase)
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
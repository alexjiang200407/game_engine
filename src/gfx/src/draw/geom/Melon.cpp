#include "gfx/draw/geom/Melon.h"
#include "gfx/bindings/Bindings.h"
#include "gfx/draw/geom/Sphere.h"

gfx::geom::Melon::Melon(
	Graphics&                              gfx,
	std::mt19937&                          rng,
	std::uniform_real_distribution<float>& adist,
	std::uniform_real_distribution<float>& ddist,
	std::uniform_real_distribution<float>& odist,
	std::uniform_real_distribution<float>& rdist,
	std::uniform_int_distribution<int>&    longdist,
	std::uniform_int_distribution<int>&    latdist) :
	DrawableBase<Melon>(gfx), r(rdist(rng)), droll(ddist(rng)), dpitch(ddist(rng)),
	dyaw(ddist(rng)), dphi(odist(rng)), dtheta(odist(rng)), dchi(odist(rng)), chi(adist(rng)),
	theta(adist(rng)), phi(adist(rng))
{
	namespace dx = DirectX;
	struct Vertex
	{
		dx::XMFLOAT3 pos;
	};
	auto model = Sphere::MakeTesselated<Vertex>(latdist(rng), longdist(rng));
	// deform vertices of model by linear transformation
	model.Transform(dx::XMMatrixScaling(1.0f, 1.0f, 1.2f));

	AddBind<VertexBuffer>(gfx, model.vertices);

	AddBind<IndexBuffer>(gfx, model.indices);

	AddBind<TransformCBuffer>(gfx, *this);
}

void
gfx::geom::Melon::Update(float dt) noexcept
{
	roll += droll * dt;
	pitch += dpitch * dt;
	yaw += dyaw * dt;
	theta += dtheta * dt;
	phi += dphi * dt;
	chi += dchi * dt;
}

DirectX::XMMATRIX
gfx::geom::Melon::GetTransformXM() const noexcept
{
	namespace dx = DirectX;
	return dx::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
	       dx::XMMatrixTranslation(r, 0.0f, 0.0f) *
	       dx::XMMatrixRotationRollPitchYaw(theta, phi, chi) *
	       dx::XMMatrixTranslation(0.0f, 0.0f, 20.0f);
}

void
gfx::geom::Melon::StaticBindingsConstructor(Graphics& gfx, DrawableBase<Melon>& melonBase)
{
	auto pvs   = melonBase.AddStaticBind<VertexShader>(gfx, L"shaders/vs_color_index.cso");
	auto pvsbc = pvs.GetBytecode();

	melonBase.AddStaticBind<PixelShader>(gfx, L"shaders/ps_color_index.cso");

	struct PixelShaderConstants
	{
		struct
		{
			float r;
			float g;
			float b;
			float a;
		} face_colors[8];
	};
	const PixelShaderConstants cb2 = { {
		{ 1.0f, 1.0f, 1.0f },
		{ 1.0f, 0.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f },
		{ 1.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 1.0f },
		{ 1.0f, 0.0f, 1.0f },
		{ 0.0f, 1.0f, 1.0f },
		{ 0.0f, 0.0f, 0.0f },
	} };
	melonBase.AddStaticBind<PixelConstantBuffer<PixelShaderConstants>>(gfx, cb2);

	const std::vector<D3D11_INPUT_ELEMENT_DESC> ied = {
		{ "Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	melonBase.AddStaticBind<InputLayout>(gfx, ied, pvsbc);
	melonBase.AddStaticBind<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
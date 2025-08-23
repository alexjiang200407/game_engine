#include "SkinnedBox.h"
#include "bindings/Bindings.h"
#include "geom/Geometry.h"

gfx::SkinnedBox::SkinnedBox(
	Graphics&                              gfx,
	std::mt19937&                          rng,
	std::uniform_real_distribution<float>& adist,
	std::uniform_real_distribution<float>& ddist,
	std::uniform_real_distribution<float>& odist,
	std::uniform_real_distribution<float>& rdist) :
	DrawableBase<SkinnedBox>(gfx), r(rdist(rng)), droll(ddist(rng)), dpitch(ddist(rng)),
	dyaw(ddist(rng)), dphi(odist(rng)), dtheta(odist(rng)), dchi(odist(rng)), chi(adist(rng)),
	theta(adist(rng)), phi(adist(rng))
{
	AddBind<TransformCBuffer>(gfx, *this);
}

void
gfx::SkinnedBox::Update(float dt) noexcept
{
	roll += droll * dt;
	pitch += dpitch * dt;
	yaw += dyaw * dt;
	theta += dtheta * dt;
	phi += dphi * dt;
	chi += dchi * dt;
}

DirectX::XMMATRIX
gfx::SkinnedBox::GetTransformXM() const noexcept
{
	namespace dx = DirectX;
	return dx::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
	       dx::XMMatrixTranslation(r, 0.0f, 0.0f) *
	       dx::XMMatrixRotationRollPitchYaw(theta, phi, chi) *
	       dx::XMMatrixTranslation(0.0f, 0.0f, 20.0f);
}

void
gfx::SkinnedBox::StaticBindingsConstructor(Graphics& gfx, DrawableBase<SkinnedBox>& boxBase)
{
	namespace dx = DirectX;
	struct Vertex
	{
		dx::XMFLOAT3 pos;
		struct
		{
			float u;
			float v;
		} tex;
	};
	const auto model = geom::Cube::MakeSkinned<Vertex>();

	boxBase.AddStaticBind<VertexBuffer>(gfx, model.vertices);

	boxBase.AddStaticBind<Texture>(gfx, L"assets/textures/cube.png", Texture::Format::kPNG);

	auto& pvs   = boxBase.AddStaticBind<VertexShader>(gfx, L"shaders/vs_texture.cso");
	auto  pvsbc = pvs.GetBytecode();

	boxBase.AddStaticBind<PixelShader>(gfx, L"shaders/ps_texture.cso");

	boxBase.AddStaticBind<Sampler>(gfx);

	boxBase.AddStaticBind<IndexBuffer>(gfx, model.indices);

	const std::vector<D3D11_INPUT_ELEMENT_DESC> ied = {
		{ "Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TexCoord", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	boxBase.AddStaticBind<InputLayout>(gfx, ied, pvsbc);

	boxBase.AddStaticBind<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
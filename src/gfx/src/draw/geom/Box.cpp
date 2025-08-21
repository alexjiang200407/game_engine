#include "gfx/draw/geom/Box.h"
#include "gfx/Graphics.h"
#include "gfx/bindings/Bindings.h"
#include "gfx/draw/geom/Cube.h"

namespace dx = DirectX;

gfx::geom::Box::Box(
	Graphics&                              gfx,
	std::mt19937&                          rng,
	std::uniform_real_distribution<float>& adist,
	std::uniform_real_distribution<float>& ddist,
	std::uniform_real_distribution<float>& odist,
	std::uniform_real_distribution<float>& rdist,
	std::uniform_real_distribution<float>& bdist) :
	r(rdist(rng)), droll(ddist(rng)), dpitch(ddist(rng)), dyaw(ddist(rng)), dphi(odist(rng)),
	dtheta(odist(rng)), dchi(odist(rng)), chi(adist(rng)), theta(adist(rng)), phi(adist(rng))
{
	if (AcquireInitialization())
	{
		struct Vertex
		{
			dx::XMFLOAT3 pos;
		};
		auto model = Cube::Make<Vertex>();

		AddStaticBind(std::make_unique<VertexBuffer>(gfx, model.vertices));

		auto pvs   = std::make_unique<VertexShader>(gfx, L"shaders/vs_color_index.cso");
		auto pvsbc = pvs->GetBytecode();
		AddStaticBind(std::move(pvs));

		AddStaticBind(std::make_unique<PixelShader>(gfx, L"shaders/ps_color_index.cso"));

		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, model.indices));

		{
			struct ConstantBuffer
			{
				struct
				{
					float r;
					float g;
					float b;
					float a;
				} face_colors[8];
			};
			const ConstantBuffer cb = { {
				{ 1.0f, 0.0f, 1.0f },
				{ 1.0f, 0.0f, 0.0f },
				{ 1.0f, 1.0f, 0.0f },
				{ 0.0f, 1.0f, 0.0f },
				{ 0.0f, 0.0f, 1.0f },
				{ 1.0f, 1.0f, 0.0f },
				{ 0.0f, 1.0f, 1.0f },
			} };
			AddStaticBind(std::make_unique<PixelConstantBuffer<ConstantBuffer>>(gfx, cb));
		}

		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied = {
			{ "Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		AddStaticBind(std::make_unique<InputLayout>(gfx, ied, pvsbc));

		AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}
	else
	{
		SetIndexFromStatic();
	}
	AddBind(std::make_unique<TransformCBuffer>(gfx, *this));

	dx::XMStoreFloat3x3(&mt, dx::XMMatrixScaling(1.0f, 1.0f, bdist(rng)));
}

void
gfx::geom::Box::Update(float dt) noexcept
{
	roll += droll * dt;
	pitch += dpitch * dt;
	yaw += dyaw * dt;
	theta += dtheta * dt;
	phi += dphi * dt;
	chi += dchi * dt;
}

DirectX::XMMATRIX
gfx::geom::Box::GetTransformXM() const noexcept
{
	namespace dx = DirectX;
	return dx::XMLoadFloat3x3(&mt) * dx::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
	       dx::XMMatrixTranslation(r, 0.0f, 0.0f) *
	       dx::XMMatrixRotationRollPitchYaw(theta, phi, chi) *
	       dx::XMMatrixTranslation(0.0f, 0.0f, 20.0f);
}
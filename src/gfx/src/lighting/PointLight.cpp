#include "lighting/PointLight.h"

gfx::PointLight::PointLight(DX11Graphics& gfx, float radius) : mesh(gfx, radius), cbuf(gfx)
{
	Reset();
}

void
gfx::PointLight::DrawControlWindow() noexcept
{
	if (ImGui::Begin("Light"))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &cbData.pos.x, -60.0f, 60.0f);
		ImGui::SliderFloat("Y", &cbData.pos.y, -60.0f, 60.0f);
		ImGui::SliderFloat("Z", &cbData.pos.z, -60.0f, 60.0f);

		ImGui::Text("Intensity/Color");
		ImGui::SliderFloat("Intensity", &cbData.diffuseIntensity, 0.01f, 2.0f);
		ImGui::ColorEdit3("Diffuse Color", &cbData.diffuseColor.x);
		ImGui::ColorEdit3("Ambient", &cbData.ambient.x);

		ImGui::Text("Falloff");
		ImGui::SliderFloat("Constant", &cbData.attConst, 0.05f, 10.0f);
		ImGui::SliderFloat("Linear", &cbData.attLin, 0.0001f, 4.0f);
		ImGui::SliderFloat("Quadratic", &cbData.attQuad, 0.0000001f, 10.0f);

		if (ImGui::Button("Reset"))
		{
			Reset();
		}
	}
	ImGui::End();
}

void
gfx::PointLight::Reset() noexcept
{
	cbData = {
		{ 0.0f, 0.0f, 0.0f },
		{ 0.05f, 0.05f, 0.05f },
		{ 1.0f, 1.0f, 1.0f },
		1.0f,
		1.0f,
		0.045f,
		0.0075f,
	};
}

void
gfx::PointLight::Draw(Graphics& gfx) const
{
	mesh.SetPos(cbData.pos);
	mesh.Draw(gfx);
}

void
gfx::PointLight::Bind(Graphics& gfx, DirectX::FXMMATRIX view) const
{
	auto       dataCopy = cbData;
	const auto pos      = DirectX::XMLoadFloat3(&cbData.pos);
	DirectX::XMStoreFloat3(&dataCopy.pos, DirectX::XMVector3Transform(pos, view));

	cbuf.Update(*gfx, dataCopy);
	cbuf.Bind(*gfx);
}

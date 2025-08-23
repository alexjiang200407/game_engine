#include "scene/Camera.h"

DirectX::XMMATRIX
scene::Camera::GetMatrix() const noexcept
{
	namespace dx = DirectX;

	const auto pos = dx::XMVector3Transform(
		dx::XMVectorSet(0.0f, 0.0f, -r, 0.0f),
		dx::XMMatrixRotationRollPitchYaw(phi, -theta, 0.0f));
	return dx::XMMatrixLookAtLH(pos, dx::XMVectorZero(), dx::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)) *
	       dx::XMMatrixRotationRollPitchYaw(pitch, -yaw, roll);
}

bool
scene::Camera::DrawControlWindow() noexcept
{
	bool modified = false;
	if (ImGui::Begin("Camera"))
	{
		ImGui::Text("Position");
		modified = ImGui::SliderFloat("R", &r, 5.0f, 80.0f, "%.1f");
		modified |= ImGui::SliderAngle("Theta", &theta, -180.0f, 180.0f);
		modified |= ImGui::SliderAngle("Phi", &phi, -89.0f, 89.0f);
		ImGui::Text("Orientation");
		modified |= ImGui::SliderAngle("Roll", &roll, -180.0f, 180.0f);
		modified |= ImGui::SliderAngle("Pitch", &pitch, -180.0f, 180.0f);
		modified |= ImGui::SliderAngle("Yaw", &yaw, -180.0f, 180.0f);
		if (ImGui::Button("Reset"))
		{
			modified = true;
			Reset();
		}
	}
	ImGui::End();
	return modified;
}

void
scene::Camera::Reset() noexcept
{
	r     = 20.0f;
	theta = 0.0f;
	phi   = 0.0f;
	pitch = 0.0f;
	yaw   = 0.0f;
	roll  = 0.0f;
}
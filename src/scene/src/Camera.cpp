#include "scene/Camera.h"

scene::Camera::Camera() noexcept { Reset(); }

DirectX::XMMATRIX
scene::Camera::GetMatrix() const noexcept
{
	namespace dx = DirectX;

	const dx::XMVECTOR forwardBaseVector = dx::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	const auto         lookVector        = dx::XMVector3Transform(
        forwardBaseVector,
        dx::XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f));
	const auto camPosition = XMLoadFloat3(&pos);
	const auto camTarget   = dx::XMVectorAdd(camPosition, lookVector);
	return dx::XMMatrixLookAtLH(camPosition, camTarget, dx::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
}

void
scene::Camera::DrawControlWindow() noexcept
{
	if (ImGui::Begin("Camera"))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &pos.x, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Y", &pos.y, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Z", &pos.z, -80.0f, 80.0f, "%.1f");
		ImGui::Text("Orientation");
		ImGui::SliderAngle("Pitch", &pitch, -180.0f, 180.0f);
		ImGui::SliderAngle("Yaw", &yaw, -180.0f, 180.0f);
		if (ImGui::Button("Reset"))
		{
			Reset();
		}
	}
	ImGui::End();
}

void
scene::Camera::Reset() noexcept
{
	pos   = { 0.0f, 7.5f, -18.0f };
	pitch = 0.0f;
	yaw   = 0.0f;
}

void
scene::Camera::Rotate(float dx, float dy) noexcept
{
	yaw   = util::math::wrap_angle(yaw + dx * rotationSpeed);
	pitch = std::clamp(pitch + dy * rotationSpeed, -util::math::PI / 2.0f, util::math::PI / 2.0f);
}

void
scene::Camera::Translate(DirectX::XMFLOAT3 translation) noexcept
{
	namespace dx = DirectX;

	dx::XMStoreFloat3(
		&translation,
		dx::XMVector3Transform(
			dx::XMLoadFloat3(&translation),
			dx::XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f) *
				dx::XMMatrixScaling(travelSpeed, travelSpeed, travelSpeed)));
	pos = { pos.x + translation.x, pos.y + translation.y, pos.z + translation.z };
}

void
scene::Camera::Consume(const wnd::KeyEvent& event, const util::Producer<wnd::KeyEvent>&)
{
	namespace dx = DirectX;

	auto translation = dx::XMFLOAT3{};

	logger::info("{}", event.code);

	if (event.type != wnd::KeyEvent::Type::kDown)
		return;

	switch (event.code)
	{
	case 87:  // W
		translation.z = travelSpeed;
		break;
	case 65:  // A
		translation.x = -travelSpeed;
		break;
	case 83:  // S
		translation.z = -travelSpeed;
		break;
	case 68:  // D
		translation.x = travelSpeed;
		break;
	default:
		break;
	}

	Translate(translation);
}

void
scene::Camera::Consume(const wnd::MouseEvent& event, const util::Producer<wnd::MouseEvent>&)
{
	using Type = wnd::MouseEvent::Type;
	namespace dx = DirectX;
	
	auto translation = dx::XMFLOAT3{};

	switch (event.type)
	{
	case Type::kMove:
		Rotate(static_cast<float>(event.dx), static_cast<float>(event.dy));
		break;
	case Type::kLPress:
		translation.y = travelSpeed;
		break;
	case Type::kRPress:
		translation.y = -travelSpeed;
		break;
	default:
		break;
	}

	Translate(translation);
}

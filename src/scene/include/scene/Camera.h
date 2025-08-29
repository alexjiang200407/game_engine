#pragma once
#include "window/Keyboard.h"
#include "window/Mouse.h"
#include <util/events/Consumer.h>

namespace scene
{
	class Camera : public util::Consumer<wnd::KeyEvent>, public util::Consumer<wnd::MouseEvent>
	{
	public:
		Camera() noexcept;

		DirectX::XMMATRIX
		GetMatrix() const noexcept;

		void
		DrawControlWindow() noexcept;

		void
		Reset() noexcept;

		void
		Consume(const wnd::KeyEvent& event, const util::Producer<wnd::KeyEvent>& producer) override;

		void
		Consume(const wnd::MouseEvent& event, const util::Producer<wnd::MouseEvent>& producer)
			override;

	private:
		void
		Rotate(float dx, float dy) noexcept;

		void
		Translate(DirectX::XMFLOAT3 translation) noexcept;

	private:
		DirectX::XMFLOAT3 pos;

		float pitch = 0.0f;
		float yaw   = 0.0f;

		static constexpr float travelSpeed   = 1.3f;
		static constexpr float rotationSpeed = 0.004f;
	};
}

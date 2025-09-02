#pragma once
#include <FidelityFX/host/ffx_fsr3.h>

namespace gfx::postprocess
{
	class FSR
	{
	public:
		// TODO: CHANGE THIS SHIT ADD EXTRA PARAMETERS AND SHIT

		FSR();
		~FSR() noexcept;

		void
		Execute();  // APPLIES FSR TECHNIQUE TO PIPELINE

	private:
		FfxFsr3Context fsrCtx;
	};
}
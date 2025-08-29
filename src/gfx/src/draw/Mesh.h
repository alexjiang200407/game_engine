#pragma once
#include "draw/DrawableBase.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace gfx
{
	class Mesh : public DrawableBase<Mesh>
	{
	public:
		Mesh(DX11Graphics& gfx, const aiMesh& mesh);

		void
		Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept;

		DirectX::XMMATRIX
		GetTransformXM() const noexcept override;

		static void
		StaticBindingsConstructor(DX11Graphics& gfx, DrawableBase<Mesh>& meshBase);

	private:
		mutable DirectX::XMFLOAT4X4 transform{};
	};
}
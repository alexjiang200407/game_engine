#pragma once
#include "bindings/Texture.h"
#include "draw/Drawable.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace gfx
{
	class Mesh : public Drawable
	{
	public:
		Mesh(
			DX11Graphics&     gfx,
			std::string_view  modelPath,
			const aiMesh&     mesh,
			const aiMaterial* material);

		void
		Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept;

		DirectX::XMMATRIX
		GetTransformXM() const noexcept override;

	private:
		bool
		AddTexture(
			std::string_view  modelPath,
			DX11Graphics&     gfx,
			aiTextureType     type,
			Texture::Slot     slot,
			const aiMaterial* material);

	private:
		mutable DirectX::XMFLOAT4X4 transform{};
	};
}
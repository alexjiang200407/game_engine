#pragma once
#include "bindings/Texture.h"
#include "draw/Drawable.h"
#include "draw/Material.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace gfx
{
	class Material;

	class Mesh : public Drawable
	{
		friend class Material;

	public:
		Mesh(
			DX11Graphics&     gfx,
			std::string_view  modelPath,
			const aiMesh&     mesh,
			const aiMaterial* material);

		void
		Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept;

		void
		DrawControlPanel(DX11Graphics& gfx);

		DirectX::XMMATRIX
		GetTransformXM() const noexcept override;

		util::cstring_view
		GetName() const noexcept
		{
			return name;
		};

	private:
		std::string                 name;
		mutable DirectX::XMFLOAT4X4 transform{};
		Material                    material;
	};
}
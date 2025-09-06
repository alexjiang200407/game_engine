#pragma once
#include "bindings/ConstantBufferEx.h"
#include "bindings/Texture.h"
#include "dcbuffer/DynamicConstant.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace gfx
{
	class Mesh;

	class Material
	{
	public:
		// Constructors must be thread safe
		Material() noexcept = default;
		Material(Mesh& mesh);
		Material(DX11Graphics& gfx, Mesh& mesh, const aiMaterial& material);

		void
		DrawSubControlPanel(Mesh& mesh) noexcept;

		std::string_view
		GetName() const noexcept
		{
			return materialName;
		};

		bool
		HasDiffuseAlpha() const noexcept;

	private:
		Texture*
		AddTexture(
			Mesh&             mesh,
			DX11Graphics&     gfx,
			aiTextureType     type,
			Texture::Slot     slot,
			const aiMaterial& material,
			bool&             hasTexture);

	private:
		std::string materialName{};
		bool        hasSpecMap = false, hasNormMap = false, hasDiffuseMap = false;
		bool        hasDiffuseAlpha = false;
	};

}
#pragma once
#include "bindings/Texture.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace gfx
{
	class Mesh;

	class Material
	{
	public:
		struct PSMaterialConstant
		{
			BOOL              normalMapEnabled  = FALSE;
			BOOL              specularEnabled   = FALSE;
			BOOL              diffuseEnabled    = FALSE;
			BOOL              hasAlpha          = FALSE;
			float             specularPower     = 5.0f;
			DirectX::XMFLOAT3 specularColor     = { 1.0f, 1.0f, 1.0f };
			float             specularMapWeight = 1.0f;
			DirectX::XMFLOAT3 diffuseColor      = { 0.45f, 0.45f, 0.85f };
		};

	public:
		Material() noexcept = default;
		Material(Mesh& mesh);
		Material(
			DX11Graphics&     gfx,
			Mesh&             mesh,
			std::string_view  modelPath,
			const aiMaterial& material);

		void
		DrawSubControlPanel(Mesh& mesh, DX11Graphics& gfx) noexcept;

		std::string_view
		GetName() const noexcept
		{
			return materialName;
		};

		const PSMaterialConstant&
		GetConstant() const noexcept
		{
			return pmc;
		}

	private:
		Texture*
		AddTexture(
			Mesh&             mesh,
			std::string_view  modelPath,
			DX11Graphics&     gfx,
			aiTextureType     type,
			Texture::Slot     slot,
			const aiMaterial& material,
			bool&             hasTexture);

	private:
		PSMaterialConstant pmc;
		std::string        materialName{};
		bool               hasSpecMap, hasNormMap, hasDiffuseMap;
	};

}
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

		void
		DrawControlPanel(DX11Graphics& gfx);

		DirectX::XMMATRIX
		GetTransformXM() const noexcept override;

	private:
		Texture*
		AddTexture(
			std::string_view  modelPath,
			DX11Graphics&     gfx,
			aiTextureType     type,
			Texture::Slot     slot,
			const aiMaterial* material,
			BOOL&             hasTexture);

	private:
		struct PSMaterialConstant
		{
			BOOL              normalMapEnabled  = FALSE;
			BOOL              specularEnabled   = FALSE;
			BOOL              diffuseEnabled    = FALSE;
			BOOL              hasAlpha          = FALSE;
			float             specularPower     = 1.0f;
			DirectX::XMFLOAT3 specularColor     = { 1.0f, 1.0f, 1.0f };
			float             specularMapWeight = 1.0f;
			DirectX::XMFLOAT3 diffuseColor      = { 0.45f, 0.45f, 0.85f };
		} pmc;
		std::string                 materialName{};
		mutable DirectX::XMFLOAT4X4 transform{};
	};
}
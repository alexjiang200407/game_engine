#include "draw/Material.h"
#include "bindings/Bindings.h"
#include "draw/Mesh.h"

gfx::Material::Material(Mesh& mesh) : materialName(mesh.GetName()) {}

gfx::Material::Material(DX11Graphics& gfx, Mesh& mesh, const aiMaterial& material)
{
	AddTexture(mesh, gfx, aiTextureType_DIFFUSE, Texture::Slot::kDiffuse, material, hasDiffuseMap);

	if (!hasDiffuseMap)
	{
		material.Get(AI_MATKEY_COLOR_DIFFUSE, reinterpret_cast<aiColor3D&>(pmc.diffuseColor));
	}

	Texture* specularTex = AddTexture(
		mesh,
		gfx,
		aiTextureType_SPECULAR,
		Texture::Slot::kSpecular,
		material,
		hasSpecMap);

	if (!hasSpecMap)
	{
		material.Get(AI_MATKEY_SHININESS, pmc.specularPower);
		material.Get(AI_MATKEY_COLOR_SPECULAR, reinterpret_cast<aiColor3D&>(pmc.specularColor));
	}
	else if (specularTex)
		pmc.hasAlpha = specularTex->HasAlpha();

	AddTexture(mesh, gfx, aiTextureType_NORMALS, Texture::Slot::kNormal, material, hasNormMap);

	mesh.AddBind<Sampler>({}, gfx);

	pmc.diffuseEnabled   = hasDiffuseMap;
	pmc.normalMapEnabled = hasNormMap;
	pmc.specularEnabled  = hasSpecMap;

	materialName = material.GetName().C_Str();
}

void
gfx::Material::DrawSubControlPanel(Mesh& mesh, DX11Graphics& gfx) noexcept
{
	if (auto pcb = mesh.QueryBindable<PixelConstantBuffer<PSMaterialConstant>>({ materialName }))
	{
		ImGui::Text("Material");
		ImGui::BeginDisabled(!hasNormMap);
		ImGui::Checkbox("Norm Map", reinterpret_cast<bool*>(&pmc.normalMapEnabled));
		ImGui::EndDisabled();

		ImGui::BeginDisabled(!hasSpecMap);
		ImGui::Checkbox("Spec Map", reinterpret_cast<bool*>(&pmc.specularEnabled));
		ImGui::EndDisabled();

		ImGui::BeginDisabled(!hasDiffuseMap);
		ImGui::Checkbox("Diffuse Map", reinterpret_cast<bool*>(&pmc.diffuseEnabled));
		ImGui::EndDisabled();

		ImGui::Checkbox("Gloss Alpha", reinterpret_cast<bool*>(&pmc.hasAlpha));
		ImGui::SliderFloat("Spec Pow", &pmc.specularPower, 1.0f, 1000.0f, "%f");

		ImGui::BeginDisabled(!pmc.specularEnabled);
		{
			ImGui::SliderFloat("Spec Weight", &pmc.specularMapWeight, 0.0f, 2.0f);
		}
		ImGui::EndDisabled();

		ImGui::BeginDisabled(pmc.specularEnabled);
		{
			ImGui::ColorPicker3("Spec Color", reinterpret_cast<float*>(&pmc.specularColor));
		}
		ImGui::EndDisabled();

		ImGui::BeginDisabled(pmc.diffuseEnabled);
		{
			ImGui::ColorPicker3("Diffuse Color", reinterpret_cast<float*>(&pmc.diffuseColor));
		}
		ImGui::EndDisabled();

		pcb->Update(gfx, pmc);
	}
}

gfx::Texture*
gfx::Material::AddTexture(
	Mesh&             mesh,
	DX11Graphics&     gfx,
	aiTextureType     type,
	Texture::Slot     slot,
	const aiMaterial& material,
	bool&             hasTexture)
{
	namespace fs = std::filesystem;

	hasTexture = false;

	aiString texFileName;
	if (material.GetTexture(type, 0, &texFileName) == AI_SUCCESS)
	{
		auto* texFileNameCStr = texFileName.C_Str();
		if (texFileNameCStr[0] == '*')
		{
			throw std::runtime_error("Embedded textures not supported");
		}
		else
		{
			Texture*   tex      = nullptr;
			const auto file     = fs::path{ texFileName.C_Str() };
			const auto fileWstr = file.wstring();
			const auto fileExt  = file.extension();

			if (fileExt == ".dds")
			{
				tex = std::addressof(mesh.AddBind<Texture>(
					{ fileWstr },
					gfx,
					fileWstr,
					Texture::Format::kDDS,
					slot));
			}
			else if (fileExt == ".png")
			{
				tex = std::addressof(mesh.AddBind<Texture>(
					{ fileWstr },
					gfx,
					fileWstr,
					Texture::Format::kPNG,
					slot));
			}
			else if (fileExt == ".jpg")
			{
				tex = std::addressof(mesh.AddBind<Texture>(
					{ fileWstr },
					gfx,
					fileWstr,
					Texture::Format::kJPG,
					slot));
			}
			else
			{
				throw std::runtime_error("Unsupported format " + fileExt.string());
			}

			if (tex)
			{
				hasTexture = true;
			}
			return tex;
		}
	}

	return nullptr;
}

#include "draw/Material.h"
#include "bindings/Bindings.h"
#include "draw/Mesh.h"

gfx::Material::Material(Mesh& mesh) : materialName(mesh.GetName()) {}

gfx::Material::Material(
	DX11Graphics&     gfx,
	Mesh&             mesh,
	std::string_view  modelPath,
	const aiMaterial& material)
{
	AddTexture(
		mesh,
		modelPath,
		gfx,
		aiTextureType_DIFFUSE,
		Texture::Slot::kDiffuse,
		material,
		pmc.diffuseEnabled);

	if (!pmc.diffuseEnabled)
	{
		material.Get(AI_MATKEY_COLOR_DIFFUSE, reinterpret_cast<aiColor3D&>(pmc.diffuseColor));
	}

	Texture* specularTex = AddTexture(
		mesh,
		modelPath,
		gfx,
		aiTextureType_SPECULAR,
		Texture::Slot::kSpecular,
		material,
		pmc.specularEnabled);

	if (!pmc.specularEnabled)
	{
		pmc.specularEnabled =
			material.Get(AI_MATKEY_SHININESS, pmc.specularPower) == aiReturn_SUCCESS;
		material.Get(AI_MATKEY_COLOR_SPECULAR, reinterpret_cast<aiColor3D&>(pmc.specularColor));
	}
	else if (specularTex)
		pmc.hasAlpha = specularTex->HasAlpha();

	AddTexture(
		mesh,
		modelPath,
		gfx,
		aiTextureType_NORMALS,
		Texture::Slot::kNormal,
		material,
		pmc.normalMapEnabled);

	if (pmc.diffuseEnabled || pmc.specularEnabled || pmc.normalMapEnabled)
		mesh.AddBind<Sampler>({}, gfx);

	materialName = material.GetName().C_Str();
}

void
gfx::Material::DrawSubControlPanel(Mesh& mesh, DX11Graphics& gfx) noexcept
{
	if (auto pcb = mesh.QueryBindable<PixelConstantBuffer<PSMaterialConstant>>({ materialName }))
	{
		ImGui::Text("Material");

		ImGui::Checkbox("Norm Map", reinterpret_cast<bool*>(&pmc.normalMapEnabled));

		ImGui::Checkbox("Spec Map", reinterpret_cast<bool*>(&pmc.specularEnabled));
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

		ImGui::Checkbox("Diffuse Map", reinterpret_cast<bool*>(&pmc.diffuseEnabled));

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
	std::string_view  modelPath,
	DX11Graphics&     gfx,
	aiTextureType     type,
	Texture::Slot     slot,
	const aiMaterial& material,
	BOOL&             hasTexture)
{
	namespace fs = std::filesystem;

	hasTexture = FALSE;

	aiString texFileName;
	if (material.GetTexture(type, 0, &texFileName) == AI_SUCCESS)
	{
		hasTexture = false;

		auto* texFileNameCStr = texFileName.C_Str();
		if (texFileNameCStr[0] == '*')
		{
			throw std::runtime_error("Embedded textures not supported");
		}
		else
		{
			fs::path path = modelPath;
			Texture* tex  = nullptr;

			if (!path.has_parent_path())
			{
				logger::warn("No parent path");
			}

			const auto file     = path.parent_path() / texFileName.C_Str();
			const auto fileWstr = file.generic_wstring();
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

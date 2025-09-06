#include "draw/Material.h"
#include "bindings/Bindings.h"
#include "draw/Mesh.h"

gfx::Material::Material(Mesh& mesh) : materialName(mesh.GetName()) {}

gfx::Material::Material(DX11Graphics& gfx, Mesh& mesh, const aiMaterial& material) :
	materialName(material.GetName().C_Str())
{
	namespace dx = DirectX;

	dcb::RawLayout layout;
	layout.Add<dcb::Bool>("normalMapEnabled");
	layout.Add<dcb::Bool>("specularMapEnabled");
	layout.Add<dcb::Bool>("diffuseMapEnabled");
	layout.Add<dcb::Bool>("hasAlpha");
	layout.Add<dcb::Float>("specularPower");
	layout.Add<dcb::Float3>("specularColor");
	layout.Add<dcb::Float>("specularMapWeight");
	layout.Add<dcb::Float3>("diffuseColor");

	auto buf = dcb::Buffer(std::move(layout));

	// Default values
	{
		buf["specularPower"]     = 5.0f;
		buf["specularColor"]     = dx::XMFLOAT3{ 1.0f, 1.0f, 1.0f };
		buf["specularMapWeight"] = 1.0f;
		buf["diffuseColor"]      = dx::XMFLOAT3{ 0.45f, 0.45f, 0.85f };
	}

	{
		auto* diffuseTex = AddTexture(
			mesh,
			gfx,
			aiTextureType_DIFFUSE,
			Texture::Slot::kDiffuse,
			material,
			hasDiffuseMap);

		if (!hasDiffuseMap)
		{
			material.Get(
				AI_MATKEY_COLOR_DIFFUSE,
				reinterpret_cast<aiColor3D&>(static_cast<DirectX::XMFLOAT3&>(buf["diffuseColor"])));
		}

		hasDiffuseAlpha = diffuseTex && diffuseTex->HasAlpha();
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
		material.Get(AI_MATKEY_SHININESS, layout["specularPower"]);
		material.Get(
			AI_MATKEY_COLOR_SPECULAR,
			reinterpret_cast<aiColor3D&>(layout["specularColor"]));
	}
	else if (specularTex)
		buf["hasAlpha"] = specularTex->HasAlpha();

	AddTexture(mesh, gfx, aiTextureType_NORMALS, Texture::Slot::kNormal, material, hasNormMap);

	mesh.AddBind<Sampler>({}, gfx);

	buf["diffuseMapEnabled"]  = hasDiffuseMap;
	buf["normalMapEnabled"]   = hasNormMap;
	buf["specularMapEnabled"] = hasSpecMap;

	const auto* pixelShader = hasDiffuseAlpha ? L"shaders/ps_lit_mask.cso" : L"shaders/ps_lit.cso";
	mesh.AddBind<PixelShader>({ pixelShader }, gfx, pixelShader);

	mesh.AddBind<CachingPixelConstantBufferEX>({ GetName() }, gfx, GetName(), buf, 1u);
}

void
gfx::Material::DrawSubControlPanel(Mesh& mesh) noexcept
{
	namespace dx = DirectX;

	if (auto handle = mesh.QueryBindable<CachingPixelConstantBufferEX>({ materialName }))
	{
		auto buf   = handle->GetBuffer();
		bool dirty = false;

		ImGui::Text("Material");

		if (auto v = buf["normalMapEnabled"]; v.Exists())
		{
			ImGui::BeginDisabled(!hasNormMap);
			dirty |= ImGui::Checkbox("Norm Map", &v);
			ImGui::EndDisabled();
		}

		if (auto v = buf["specularMapEnabled"]; v.Exists())
		{
			ImGui::BeginDisabled(!hasSpecMap);
			dirty |= ImGui::Checkbox("Spec Map", &v);
			ImGui::EndDisabled();
			if (auto v2 = buf["specularPower"]; v2.Exists())
			{
				dirty |= ImGui::SliderFloat("Spec Pow", &v2, 1.0f, 1000.0f, "%f");
			}
			ImGui::BeginDisabled(!v);
			{
				if (auto v2 = buf["hasAlpha"]; v2.Exists())
				{
					dirty |= ImGui::Checkbox("Gloss Alpha", &v2);
				}
				if (auto v3 = buf["specularMapWeight"]; v3.Exists())
				{
					dirty |=
						ImGui::SliderFloat("Spec Weight", &buf["specularMapWeight"], 0.0f, 2.0f);
				}
			}
			ImGui::EndDisabled();
		}

		if (auto v = buf["diffuseMapEnabled"]; v.Exists())
		{
			ImGui::BeginDisabled(!hasDiffuseMap);
			dirty |= ImGui::Checkbox("Diffuse Map", &v);
			ImGui::EndDisabled();

			ImGui::BeginDisabled(v);
			{
				if (auto v2 = buf["diffuseColor"]; v2.Exists())
				{
					dirty |= ImGui::ColorPicker3(
						"Diffuse Color",
						reinterpret_cast<float*>(&static_cast<dx::XMFLOAT3&>(v2)));
				}
			}
			ImGui::EndDisabled();
		}

		handle->SetBuffer(buf);
	}
}

bool
gfx::Material::HasDiffuseAlpha() const noexcept
{
	return hasDiffuseAlpha;
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

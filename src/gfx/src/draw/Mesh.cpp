#include "draw/Mesh.h"
#include "bindings/Bindings.h"
#include "draw/Codex.h"
#include "geom/Geometry.h"

using namespace std::literals;

gfx::Mesh::Mesh(
	DX11Graphics&     gfx,
	std::string_view  modelPath,
	const aiMesh&     mesh,
	const aiMaterial* material)
{
	namespace dx      = DirectX;
	using ElementType = geom::VertexLayout::ElementType;

	geom::VertexBuffer vbuf(std::move(geom::VertexLayout{}
	                                      .Append(ElementType::Position3D)
	                                      .Append(ElementType::Normal)
	                                      .Append(ElementType::Tangent)
	                                      .Append(ElementType::BiTangent)
	                                      .Append(ElementType::Texture2D)));

	for (unsigned int i = 0; i < mesh.mNumVertices; i++)
	{
		vbuf.EmplaceBack(
			*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mVertices[i]),
			*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mNormals[i]),
			*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mTangents[i]),
			*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mBitangents[i]),
			*reinterpret_cast<dx::XMFLOAT2*>(&mesh.mTextureCoords[0][i]));
	}

	std::vector<unsigned short> indices;
	indices.reserve(mesh.mNumFaces * 3ull);
	for (unsigned int i = 0; i < mesh.mNumFaces; i++)
	{
		const auto& face = mesh.mFaces[i];
		assert(face.mNumIndices == 3);
		indices.push_back(static_cast<unsigned short>(face.mIndices[0]));
		indices.push_back(static_cast<unsigned short>(face.mIndices[1]));
		indices.push_back(static_cast<unsigned short>(face.mIndices[2]));
	}

	// Load Textures
	if (material)
	{
		AddTexture(
			modelPath,
			gfx,
			aiTextureType_DIFFUSE,
			Texture::Slot::kDiffuse,
			material,
			pmc.diffuseEnabled);

		if (!pmc.diffuseEnabled)
		{
			material->Get(AI_MATKEY_COLOR_DIFFUSE, reinterpret_cast<aiColor3D&>(pmc.diffuseColor));
		}

		Texture* specularTex = AddTexture(
			modelPath,
			gfx,
			aiTextureType_SPECULAR,
			Texture::Slot::kSpecular,
			material,
			pmc.specularEnabled);

		if (!pmc.specularEnabled)
		{
			pmc.specularEnabled =
				material->Get(AI_MATKEY_SHININESS, pmc.specularPower) == aiReturn_SUCCESS;
			material->Get(
				AI_MATKEY_COLOR_SPECULAR,
				reinterpret_cast<aiColor3D&>(pmc.specularColor));
		}
		else if (specularTex)
			pmc.hasAlpha = specularTex->HasAlpha();

		AddTexture(
			modelPath,
			gfx,
			aiTextureType_NORMALS,
			Texture::Slot::kNormal,
			material,
			pmc.normalMapEnabled);

		if (pmc.diffuseEnabled || pmc.specularEnabled || pmc.normalMapEnabled)
			AddBind<Sampler>({}, gfx);

		materialName = material->GetName().C_Str();
	}

	{
		auto tag = std::string_view(mesh.mName.C_Str(), mesh.mName.length);

		AddBind<VertexBuffer>({ modelPath, tag }, gfx, vbuf, modelPath, tag);
		AddBind<IndexBuffer>({ modelPath, tag }, gfx, indices, modelPath, tag);
	}

	static constexpr const auto* vertexShader = L"shaders/vs_lit.cso";

	auto& pvs   = AddBind<VertexShader>({ vertexShader }, gfx, vertexShader);
	auto  pvsbc = pvs.GetBytecode();

	const auto& layout = vbuf.GetLayout();
	AddBind<InputLayout>({ layout }, gfx, layout, pvsbc);

	{
		static constexpr const auto* pixelShader = L"shaders/ps_lit.cso";
		AddBind<PixelShader>({ pixelShader }, gfx, pixelShader);
		auto tag = material && material->GetName().C_Str() ?
		               std::string_view(material->GetName().C_Str(), material->GetName().length) :
		               ""sv;
		AddBind<PixelConstantBuffer<PSMaterialConstant>>({ tag }, gfx, tag, pmc, 1u);
	}

	AddBind<Topology>(
		{ static_cast<int>(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) },
		gfx,
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	AddUniqueBind<TransformCBuffer>(gfx, *this);
}

DirectX::XMMATRIX
gfx::Mesh::GetTransformXM() const noexcept
{
	return DirectX::XMLoadFloat4x4(&transform);
}

void
gfx::Mesh::Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept
{
	DirectX::XMStoreFloat4x4(&transform, accumulatedTransform);
	Drawable::Draw(gfx);
}

void
gfx::Mesh::DrawControlPanel(DX11Graphics& gfx)
{
	if (auto pcb = QueryBindable<PixelConstantBuffer<PSMaterialConstant>>({ materialName }))
	{
		ImGui::Text("Material");

		ImGui::Checkbox("Norm Map", reinterpret_cast<bool*>(&pmc.normalMapEnabled));

		ImGui::Checkbox("Spec Map", reinterpret_cast<bool*>(&pmc.specularEnabled));
		ImGui::Checkbox("Gloss Alpha", reinterpret_cast<bool*>(&pmc.hasAlpha));
		ImGui::SliderFloat("Spec Pow", &pmc.specularPower, 0.1f, 1000.0f, "%f");

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
gfx::Mesh::AddTexture(
	std::string_view  modelPath,
	DX11Graphics&     gfx,
	aiTextureType     type,
	Texture::Slot     slot,
	const aiMaterial* material,
	BOOL&             hasTexture)
{
	namespace fs = std::filesystem;

	hasTexture = FALSE;

	aiString texFileName;
	if (material->GetTexture(type, 0, &texFileName) == AI_SUCCESS)
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
				tex = std::addressof(
					AddBind<Texture>({ fileWstr }, gfx, fileWstr, Texture::Format::kDDS, slot));
			}
			else if (fileExt == ".png")
			{
				tex = std::addressof(
					AddBind<Texture>({ fileWstr }, gfx, fileWstr, Texture::Format::kPNG, slot));
			}
			else if (fileExt == ".jpg")
			{
				tex = std::addressof(
					AddBind<Texture>({ fileWstr }, gfx, fileWstr, Texture::Format::kJPG, slot));
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

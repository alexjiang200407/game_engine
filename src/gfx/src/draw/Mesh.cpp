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
	                                      .Append(ElementType::Texture2D)));

	for (unsigned int i = 0; i < mesh.mNumVertices; i++)
	{
		vbuf.EmplaceBack(
			*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mVertices[i]),
			*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mNormals[i]),
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

	bool  hasSpecular = false, hasDiffuse = false;
	float shininess = 35.0f;

	// Load Textures
	if (material)
	{
		hasDiffuse =
			AddTexture(modelPath, gfx, aiTextureType_DIFFUSE, Texture::Slot::kDiffuse, material);

		hasSpecular =
			AddTexture(modelPath, gfx, aiTextureType_SPECULAR, Texture::Slot::kSpecular, material);

		if (hasDiffuse || hasSpecular)
			AddBind<Sampler>({}, gfx);

		if (!hasSpecular)
			material->Get(AI_MATKEY_SHININESS, shininess);
	}

	auto tag = std::string_view(mesh.mName.C_Str(), mesh.mName.length);

	AddBind<VertexBuffer>({ modelPath, tag }, gfx, vbuf, modelPath, tag);
	AddBind<IndexBuffer>({ modelPath, tag }, gfx, indices, modelPath, tag);

	static constexpr const auto* vertexShader = L"shaders/vs_phong.cso";

	auto& pvs   = AddBind<VertexShader>({ vertexShader }, gfx, vertexShader);
	auto  pvsbc = pvs.GetBytecode();

	const auto& layout = vbuf.GetLayout();
	AddBind<InputLayout>({ layout }, gfx, layout, pvsbc);

	if (hasSpecular)
	{
		static constexpr const auto* pixelShader = L"shaders/ps_phong_specular.cso";
		AddBind<PixelShader>({ pixelShader }, gfx, pixelShader);
	}
	else
	{
		static constexpr const auto* pixelShader = L"shaders/ps_phong.cso";
		AddBind<PixelShader>({ pixelShader }, gfx, pixelShader);

		struct PSMaterialConstant
		{
			float specularIntensity = 1.6f;
			float specularPower;
			float padding[2];
		} pmc;

		pmc.specularPower = shininess;

		AddBind<PixelConstantBuffer<PSMaterialConstant>>({ 1u }, gfx, pmc, 1u);
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

bool
gfx::Mesh::AddTexture(
	std::string_view  modelPath,
	DX11Graphics&     gfx,
	aiTextureType     type,
	Texture::Slot     slot,
	const aiMaterial* material)
{
	namespace fs = std::filesystem;

	aiString texFileName;
	if (material->GetTexture(type, 0, &texFileName) == AI_SUCCESS)
	{
		auto* texFileNameCStr = texFileName.C_Str();
		if (texFileNameCStr[0] == '*')
		{
			throw std::runtime_error("Embedded textures not supported");
		}
		else
		{
			fs::path path = modelPath;

			if (!path.has_parent_path())
			{
				logger::warn("No parent path");
			}

			const auto file     = path.parent_path() / texFileName.C_Str();
			const auto fileWstr = file.generic_wstring();
			const auto fileExt  = file.extension();

			if (fileExt == ".dds")
			{
				AddBind<Texture>({ fileWstr }, gfx, fileWstr, Texture::Format::kDDS, slot);
			}
			else if (fileExt == ".png")
			{
				AddBind<Texture>({ fileWstr }, gfx, fileWstr, Texture::Format::kPNG, slot);
			}
			else
			{
				throw std::runtime_error("Unsupported format " + fileExt.string());
			}
		}
		return true;
	}

	return false;
}

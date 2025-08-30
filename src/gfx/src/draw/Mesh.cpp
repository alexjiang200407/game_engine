#include "draw/Mesh.h"
#include "bindings/Bindings.h"
#include "geom/Geometry.h"

gfx::Mesh::Mesh(
	DX11Graphics&           gfx,
	std::string_view        modelPath,
	const aiMesh&           mesh,
	const aiMaterial*       material,
	const aiTexture* const* textureArray) : DrawableBase<Mesh>(gfx)
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
		hasDiffuse = AddTexture(
			modelPath,
			gfx,
			aiTextureType_DIFFUSE,
			Texture::Slot::kDiffuse,
			material,
			textureArray);

		hasSpecular = AddTexture(
			modelPath,
			gfx,
			aiTextureType_SPECULAR,
			Texture::Slot::kSpecular,
			material,
			textureArray);

		if (hasDiffuse || hasSpecular)
			AddBind<Sampler>(gfx);

		if (!hasSpecular)
			material->Get(AI_MATKEY_SHININESS, shininess);
	}

	AddBind<VertexBuffer>(gfx, vbuf);
	AddBind<IndexBuffer>(gfx, indices);

	auto& pvs   = AddBind<VertexShader>(gfx, L"shaders/vs_phong.cso");
	auto  pvsbc = pvs.GetBytecode();

	if (hasSpecular)
		AddBind<PixelShader>(gfx, L"shaders/ps_phong_specular.cso");
	else
		AddBind<PixelShader>(gfx, L"shaders/ps_phong.cso");

	AddBind<InputLayout>(gfx, vbuf.GetLayout().GetD3DLayout(), pvsbc);

	struct PSMaterialConstant
	{
		float specularIntensity = 1.6f;
		float specularPower;
		float padding[2];
	} pmc;

	pmc.specularPower = shininess;

	AddBind<PixelConstantBuffer<PSMaterialConstant>>(gfx, pmc, 1u);
	AddBind<TransformCBuffer>(gfx, *this);
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
gfx::Mesh::StaticBindingsConstructor(DX11Graphics& gfx, DrawableBase<Mesh>& meshBase)
{
	meshBase.AddStaticBind<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

bool
gfx::Mesh::AddTexture(
	std::string_view        modelPath,
	DX11Graphics&           gfx,
	aiTextureType           type,
	Texture::Slot           slot,
	const aiMaterial*       material,
	const aiTexture* const* textureArray)
{
	namespace fs = std::filesystem;

	aiString texFileName;
	if (material->GetTexture(type, 0, &texFileName) == AI_SUCCESS)
	{
		auto* texFileNameCStr = texFileName.C_Str();
		if (texFileNameCStr[0] == '*' && textureArray)
		{
			// Handle embedded textures
			// TODO: Handle compression
			int   texIndex = atoi(&texFileNameCStr[1]);
			auto* tex      = textureArray[texIndex];
			AddBind<Texture>(
				gfx,
				reinterpret_cast<const unsigned char*>(tex->pcData),
				tex->mWidth,
				Texture::Format::kPNG,
				slot);
		}
		else
		{
			fs::path path = modelPath;

			if (!path.has_parent_path())
			{
				logger::warn("No parent path");
			}

			const auto file = path.parent_path() / texFileName.C_Str();
			AddBind<Texture>(gfx, file.wstring(), Texture::Format::kPNG, slot);
		}
		return true;
	}

	return false;
}

#include "draw/Mesh.h"
#include "bindings/Bindings.h"
#include "draw/Codex.h"
#include "geom/Geometry.h"

using namespace std::literals;

gfx::Mesh::Mesh(
	DX11Graphics&     gfx,
	std::string_view  modelPath,
	const aiMesh&     mesh,
	const aiMaterial* a_material) : name(mesh.mName.C_Str())
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

	if (a_material)
	{
		material = Material(gfx, *this, *a_material);
	}
	else
	{
		material = Material(*this);
	}

	{
		AddBind<VertexBuffer>({ modelPath, name }, gfx, vbuf, modelPath, name);
		AddBind<IndexBuffer>({ modelPath, name }, gfx, indices, modelPath, name);
	}

	static constexpr const auto* vertexShader = L"shaders/vs_lit.cso";

	auto& pvs   = AddBind<VertexShader>({ vertexShader }, gfx, vertexShader);
	auto  pvsbc = pvs.GetBytecode();

	const auto& layout = vbuf.GetLayout();
	AddBind<InputLayout>({ layout }, gfx, layout, pvsbc);

	const auto* pixelShader =
		material.HasDiffuseAlpha() ? L"shaders/ps_lit_mask.cso" : L"shaders/ps_lit.cso";
	AddBind<PixelShader>({ pixelShader }, gfx, pixelShader);
	AddBind<PixelConstantBuffer<Material::PSMaterialConstant>>(
		{ material.GetName() },
		gfx,
		material.GetName(),
		material.GetConstant(),
		1u);

	AddBind<Topology>(
		{ static_cast<int>(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) },
		gfx,
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//AddBind<Blender>({ material.HasDiffuseAlpha() }, gfx, material.HasDiffuseAlpha());
	AddBind<Rasterizer>({ material.HasDiffuseAlpha() }, gfx, material.HasDiffuseAlpha());

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
	material.DrawSubControlPanel(*this, gfx);
}

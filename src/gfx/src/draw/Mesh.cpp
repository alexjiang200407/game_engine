#include "draw/Mesh.h"
#include "bindings/Bindings.h"
#include "geom/Geometry.h"

gfx::Mesh::Mesh(DX11Graphics& gfx, const aiMesh& mesh) : DrawableBase<Mesh>(gfx)
{
	namespace dx      = DirectX;
	using ElementType = geom::VertexLayout::ElementType;

	geom::VertexBuffer vbuf(std::move(
		geom::VertexLayout{}.Append(ElementType::Position3D).Append(ElementType::Normal)));

	for (unsigned int i = 0; i < mesh.mNumVertices; i++)
	{
		vbuf.EmplaceBack(
			*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mVertices[i]),
			*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mNormals[i]));
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

	AddBind<VertexBuffer>(gfx, vbuf);
	AddBind<IndexBuffer>(gfx, indices);

	auto& pvs   = AddBind<VertexShader>(gfx, L"shaders/vs_phong.cso");
	auto  pvsbc = pvs.GetBytecode();

	AddBind<PixelShader>(gfx, L"shaders/ps_phong.cso");

	AddBind<InputLayout>(gfx, vbuf.GetLayout().GetD3DLayout(), pvsbc);

	struct PSMaterialConstant
	{
		DirectX::XMFLOAT3 color             = { 0.6f, 0.6f, 0.8f };
		float             specularIntensity = 0.6f;
		float             specularPower     = 30.0f;
		float             padding[3];
	} pmc;
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
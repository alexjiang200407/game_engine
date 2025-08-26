#include "draw/Mesh.h"
#include "bindings/Bindings.h"
#include "geom/Geometry.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

gfx::Mesh::Mesh(
	DX11Graphics&                          gfx,
	const std::string&                     path,
	DirectX::XMFLOAT3                      material,
	std::mt19937&                          rng,
	std::uniform_real_distribution<float>& adist,
	std::uniform_real_distribution<float>& ddist,
	std::uniform_real_distribution<float>& odist,
	std::uniform_real_distribution<float>& rdist,
	std::uniform_real_distribution<float>& bdist,
	float                                  scale) :
	DrawableBase<Mesh>(gfx), r(rdist(rng)), droll(ddist(rng)), dpitch(ddist(rng)), dyaw(ddist(rng)),
	dphi(odist(rng)), dtheta(odist(rng)), dchi(odist(rng)), chi(adist(rng)), theta(adist(rng)),
	phi(adist(rng))
{
	namespace dx = DirectX;
	Assimp::Importer imp;
	const auto       pModel =
		imp.ReadFile(path.c_str(), aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);

	struct PSMaterialConstant
	{
		DirectX::XMFLOAT3 color             = {};
		float             specularIntensity = 0.6f;
		float             specularPower     = 30.0f;
		float             padding[3];
	} pmc;
	pmc.color = material;
	AddBind<PixelConstantBuffer<PSMaterialConstant>>(gfx, pmc, 1u);

	using ElementType = geom::VertexLayout::ElementType;

	geom::VertexBuffer vbuf(std::move(
		geom::VertexLayout{}.Append(ElementType::Position3D).Append(ElementType::Normal)));

	if (pModel->HasMeshes())
	{
		std::vector<unsigned short> indices{};

		const auto pMesh = pModel->mMeshes[0];
		vbuf.Reserve(pMesh->mNumVertices);

		for (unsigned int i = 0; i < pMesh->mNumVertices; i++)
		{
			vbuf.EmplaceBack(
				dx::XMFLOAT3{ pMesh->mVertices[i].x * scale,
			                  pMesh->mVertices[i].y * scale,
			                  pMesh->mVertices[i].z * scale },
				*reinterpret_cast<dx::XMFLOAT3*>(&pMesh->mNormals[i]));
		}

		indices.reserve(pMesh->mNumFaces * 3ull);
		for (unsigned int i = 0; i < pMesh->mNumFaces; i++)
		{
			const auto& face = pMesh->mFaces[i];
			assert(face.mNumIndices == 3);
			indices.push_back(static_cast<unsigned short>(face.mIndices[0]));
			indices.push_back(static_cast<unsigned short>(face.mIndices[1]));
			indices.push_back(static_cast<unsigned short>(face.mIndices[2]));
		}

		AddBind<VertexBuffer>(gfx, vbuf);
		AddBind<IndexBuffer>(gfx, indices);
	}

	if (pModel->HasTextures() && false)
	{
		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied = {
			{ "Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "Normal",
			  0,
			  DXGI_FORMAT_R32G32B32_FLOAT,
			  0,
			  D3D11_APPEND_ALIGNED_ELEMENT,
			  D3D11_INPUT_PER_VERTEX_DATA,
			  0 },
			{ "TexCoord",
			  0,
			  DXGI_FORMAT_R32G32_FLOAT,
			  0,
			  D3D11_APPEND_ALIGNED_ELEMENT,
			  D3D11_INPUT_PER_VERTEX_DATA,
			  0 },
		};

		auto* tex     = pModel->mTextures[0];
		auto  texData = std::span<const unsigned char>(
            reinterpret_cast<unsigned char*>(tex->pcData),
            tex->mWidth);
		if (tex->CheckFormat("png"))
		{
			AddBind<Texture>(gfx, texData, Texture::Format::kPNG);
		}
		else if (tex->CheckFormat("jpg"))
		{
			AddBind<Texture>(gfx, texData, Texture::Format::kJPG);
		}
		else
		{
			AddBind<Texture>(gfx, texData, Texture::Format::kDDS);
		}
		AddBind<Sampler>(gfx);
		auto& pvs   = AddBind<VertexShader>(gfx, L"shaders/vs_textured_phong.cso");
		auto  pvsbc = pvs.GetBytecode();
		AddBind<PixelShader>(gfx, L"shaders/ps_textured_phong.cso");
		AddBind<InputLayout>(gfx, ied, pvsbc);
		AddBind<InputLayout>(gfx, ied, pvsbc);
	}
	else
	{
		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied = {
			{ "Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "Normal",
			  0,
			  DXGI_FORMAT_R32G32B32_FLOAT,
			  0,
			  D3D11_APPEND_ALIGNED_ELEMENT,
			  D3D11_INPUT_PER_VERTEX_DATA,
			  0 },
		};

		auto& pvs   = AddBind<VertexShader>(gfx, L"shaders/vs_phong.cso");
		auto  pvsbc = pvs.GetBytecode();
		AddBind<PixelShader>(gfx, L"shaders/ps_phong.cso");
		AddBind<InputLayout>(gfx, ied, pvsbc);
	}

	AddBind<TransformCBuffer>(gfx, *this);
	dx::XMStoreFloat3x3(&mt, dx::XMMatrixScaling(1.0f, 1.0f, bdist(rng)));
}

void
gfx::Mesh::StaticBindingsConstructor(DX11Graphics& gfx, DrawableBase<Mesh>& meshBase)
{
	meshBase.AddStaticBind<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void
gfx::Mesh::Update(float dt) noexcept
{
	roll += droll * dt;
	pitch += dpitch * dt;
	yaw += dyaw * dt;
	theta += dtheta * dt;
	phi += dphi * dt;
	chi += dchi * dt;
}

DirectX::XMMATRIX
gfx::Mesh::GetTransformXM() const noexcept
{
	namespace dx = DirectX;
	return dx::XMLoadFloat3x3(&mt) * dx::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
	       dx::XMMatrixTranslation(r, 0.0f, 0.0f) *
	       dx::XMMatrixRotationRollPitchYaw(theta, phi, chi);
}

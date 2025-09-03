#include "Model.h"
#include "DX11Graphics.h"
#include "bindings/Bindings.h"
#include "geom/Geometry.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace
{
	DirectX::XMFLOAT3
	ExtractEulerAngles(const DirectX::XMFLOAT4X4& mat)
	{
		DirectX::XMFLOAT3 euler{};

		euler.x = asinf(-mat._32);   // Pitch
		if (cosf(euler.x) > 0.0001)  // Not at poles
		{
			euler.y = atan2f(mat._31, mat._33);  // Yaw
			euler.z = atan2f(mat._12, mat._22);  // Roll
		}
		else
		{
			euler.y = 0.0f;                       // Yaw
			euler.z = atan2f(-mat._21, mat._11);  // Roll
		}

		return euler;
	}

	DirectX::XMFLOAT3
	ExtractTranslation(const DirectX::XMFLOAT4X4& matrix)
	{
		return { matrix._41, matrix._42, matrix._43 };
	}
}

std::unique_ptr<gfx::Node>
gfx::Node::ParseNode(
	const std::vector<std::unique_ptr<gfx::Mesh>>& meshPtrs,
	const aiNode&                                  root) noexcept
{
	namespace dx = DirectX;

	const auto rootTransform = dx::XMMatrixTranspose(
		dx::XMLoadFloat4x4(reinterpret_cast<const dx::XMFLOAT4X4*>(&root.mTransformation)));

	auto pRoot = std::make_unique<gfx::Node>(
		0u,
		root.mName.C_Str(),
		std::vector<gfx::Mesh*>{},
		rootTransform);

	if (root.mNumMeshes > 0)
	{
		std::vector<gfx::Mesh*> curMeshPtrs;
		curMeshPtrs.reserve(root.mNumMeshes);
		for (size_t i = 0; i < root.mNumMeshes; i++)
		{
			curMeshPtrs.push_back(meshPtrs.at(root.mMeshes[i]).get());
		}
		pRoot->meshPtrs = curMeshPtrs;
	}

	std::stack<std::pair<const aiNode*, gfx::Node*>> stack;
	stack.push({ &root, pRoot.get() });

	for (size_t idx = 1u; !stack.empty();)
	{
		auto [aiParent, gfxParent] = std::move(stack.top());
		stack.pop();

		for (size_t i = 0; i < aiParent->mNumChildren; ++i, ++idx)
		{
			const aiNode* aiChild = aiParent->mChildren[i];

			const auto childTransform = dx::XMMatrixTranspose(dx::XMLoadFloat4x4(
				reinterpret_cast<const dx::XMFLOAT4X4*>(&aiChild->mTransformation)));

			std::vector<gfx::Mesh*> childMeshes;
			childMeshes.reserve(aiChild->mNumMeshes);
			for (size_t j = 0; j < aiChild->mNumMeshes; j++)
			{
				childMeshes.push_back(meshPtrs.at(aiChild->mMeshes[j]).get());
			}

			auto pChildNode =
				std::make_unique<Node>(idx, aiChild->mName.C_Str(), childMeshes, childTransform);
			Node* childRaw = pChildNode.get();

			gfxParent->AddChild(std::move(pChildNode));

			stack.push({ aiChild, childRaw });
		}
	}

	return pRoot;
}

void
gfx::Node::DrawNodeHierarchyPanel(Node*& pSelectedNode) const
{
	std::stack<std::tuple<const Node*, size_t>> stack;
	stack.push({ this, 0 });

	for (size_t currentNode = 0u; !stack.empty(); ++currentNode)
	{
		auto [node, popParent] = std::move(stack.top());
		stack.pop();

		const auto selectedIdx =
			pSelectedNode ? pSelectedNode->idx : std::numeric_limits<size_t>::max();
		const auto nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow |
		                       ((selectedIdx == currentNode) ? ImGuiTreeNodeFlags_Selected : 0) |
		                       ((node->childPtrs.size() == 0) ? ImGuiTreeNodeFlags_Leaf : 0);

		if (ImGui::TreeNodeEx(node->name.c_str(), nodeFlags))
		{
			if (node->childPtrs.empty())
			{
				ImGui::TreePop();

				if (popParent)
				{
					for (size_t i = 0; i < popParent; ++i)
					{
						ImGui::TreePop();
					}
				}
			}
			else
			{
				for (auto it = node->childPtrs.rbegin(); it != node->childPtrs.rend(); ++it)
				{
					bool isLast = (it == node->childPtrs.rbegin());
					stack.push({ it->get(), isLast ? popParent + 1u : 0u });
				}
			}
		}
		else if (popParent)
		{
			for (size_t i = 0; i < popParent; ++i)
			{
				ImGui::TreePop();
			}
		}

		if (ImGui::IsItemClicked())
		{
			pSelectedNode = const_cast<Node*>(node);
		}
	}
}

void
gfx::Model::DrawControlPanel(
	Graphics&                         gfx,
	std::optional<util::cstring_view> imguiIDOverride) noexcept
{
	if (ImGui::Begin(imguiIDOverride.has_value() ? imguiIDOverride->c_str() : imguiID.c_str()))
	{
		ImGui::Columns(2, "", true);

		pRoot->DrawNodeHierarchyPanel(pSelectedNode);

		if (pSelectedNode)
		{
			const auto id = pSelectedNode->idx;
			auto       i  = transforms.find(id);
			if (i == transforms.end())
			{
				const auto&         applied     = pSelectedNode->GetAppliedTransform();
				const auto          angles      = ExtractEulerAngles(applied);
				const auto          translation = ExtractTranslation(applied);
				TransformParameters tp;
				tp.roll                  = angles.z;
				tp.pitch                 = angles.x;
				tp.yaw                   = angles.y;
				tp.x                     = translation.x;
				tp.y                     = translation.y;
				tp.z                     = translation.z;
				std::tie(i, std::ignore) = transforms.insert({ id, tp });
			}
			auto& transform = i->second;

			ImGui::NextColumn();
			ImGui::Text("Orientation");
			ImGui::SliderAngle("Roll", &transform.roll, -180.0f, 180.0f);
			ImGui::SliderAngle("Pitch", &transform.pitch, -180.0f, 180.0f);
			ImGui::SliderAngle("Yaw", &transform.yaw, -180.0f, 180.0f);
			ImGui::Text("Position");
			ImGui::SliderFloat("X", &transform.x, -20.0f, 20.0f);
			ImGui::SliderFloat("Y", &transform.y, -20.0f, 20.0f);
			ImGui::SliderFloat("Z", &transform.z, -20.0f, 20.0f);

			if (ImGui::Button("Reset"))
			{
				transform = {};
			}

			if (!pSelectedNode->meshPtrs.empty())
				pSelectedNode->meshPtrs.front()->DrawControlPanel(*gfx);
		}
	}
	ImGui::End();
}

void
gfx::Node::AddChild(std::unique_ptr<Node> pChild)
{
	childPtrs.push_back(std::move(pChild));
}

gfx::Node::Node(
	std::size_t              a_idx,
	std::string_view         a_name,
	std::vector<Mesh*>       meshPtrs,
	const DirectX::XMMATRIX& transform) noexcept :
	idx(a_idx), name(a_name), meshPtrs(std::move(meshPtrs))
{
	namespace dx = DirectX;
	dx::XMStoreFloat4x4(&baseTransform, transform);
	dx::XMStoreFloat4x4(&appliedTransform, dx::XMMatrixIdentity());
}

void
gfx::Node::Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept
{
	namespace dx     = DirectX;
	const auto built = dx::XMLoadFloat4x4(&baseTransform) * dx::XMLoadFloat4x4(&appliedTransform) *
	                   accumulatedTransform;
	for (const auto pm : meshPtrs)
	{
		pm->Draw(gfx, built);
	}
	for (const auto& pc : childPtrs)
	{
		pc->Draw(gfx, built);
	}
}

void
gfx::Node::SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept
{
	namespace dx = DirectX;
	dx::XMStoreFloat4x4(&appliedTransform, transform);
}

const DirectX::XMFLOAT4X4&
gfx::Node::GetAppliedTransform() const noexcept
{
	return appliedTransform;
}

gfx::Model::Model(Graphics& gfx, std::string_view a_fileName) :
	fileName(a_fileName), imguiID(fileName + "###scenegraph")
{
	Assimp::Importer imp;
	const auto       pScene = imp.ReadFile(
        fileName.c_str(),
        aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_ConvertToLeftHanded |
            aiProcess_GenNormals | aiProcess_CalcTangentSpace);

	if (!pScene)
		throw std::runtime_error("Could not load scene "s + imp.GetErrorString());

	{
		auto fileNameSv = std::string_view(fileName);
		for (size_t i = 0; i < pScene->mNumMeshes; i++)
		{
			auto idx = pScene->mMeshes[i]->mMaterialIndex;
			allMeshes.push_back(std::make_unique<Mesh>(
				*gfx,
				fileNameSv,
				*pScene->mMeshes[i],
				idx >= 0 ? pScene->mMaterials[idx] : nullptr));
		}
	}

	pRoot = Node::ParseNode(allMeshes, *pScene->mRootNode);
}

void
gfx::Model::Draw(Graphics& gfx) const noexcept
{
	namespace dx = DirectX;
	if (pSelectedNode)
	{
		pSelectedNode->SetAppliedTransform(GetTransform());
	}
	pRoot->Draw(gfx, dx::XMMatrixIdentity());
}

DirectX::XMMATRIX
gfx::Model::GetTransform() const noexcept
{
	namespace dx          = DirectX;
	const auto& transform = transforms.at(pSelectedNode->idx);
	return dx::XMMatrixRotationRollPitchYaw(transform.roll, transform.pitch, transform.yaw) *
	       dx::XMMatrixTranslation(transform.x, transform.y, transform.z);
}

void
gfx::Model::SetRootTransform(DirectX::FXMMATRIX tf) const noexcept
{
	pRoot->SetAppliedTransform(tf);
}

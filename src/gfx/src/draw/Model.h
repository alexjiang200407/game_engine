#pragma once
#include "draw/Mesh.h"
#include "geom/Geometry.h"
#include <gfx/IModel.h>

namespace gfx
{
	class Model;

	class Node
	{
		friend class Model;

	public:
		Node(
			std::size_t              idx,
			std::string_view         name,
			std::vector<Mesh*>       meshPtrs,
			const DirectX::XMMATRIX& transform) noexcept;

	private:
		void
		Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept;

		static std::unique_ptr<Node>
		ParseNode(
			const std::vector<std::unique_ptr<gfx::Mesh>>& meshPtrs,
			const aiNode&                                  node) noexcept;

		void
		DrawNodeHierarchyPanel(Node*& pSelectedNode) const;

		void
		SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept;

		const DirectX::XMFLOAT4X4&
		GetAppliedTransform() const noexcept;

		void
		AddChild(std::unique_ptr<Node> pChild);

	private:
		struct
		{
			float roll  = 0.0f;
			float pitch = 0.0f;
			float yaw   = 0.0f;
			float x     = 0.0f;
			float y     = 0.0f;
			float z     = 0.0f;
		} pos;

		size_t                             idx;
		std::string                        name;
		std::vector<std::unique_ptr<Node>> childPtrs;
		std::vector<Mesh*>                 meshPtrs;
		DirectX::XMFLOAT4X4                baseTransform;
		DirectX::XMFLOAT4X4                appliedTransform;
	};

	class Model : public IModel
	{
	public:
		Model(Graphics& gfx, std::string_view fileName);

		void
		DrawControlPanel(Graphics& gfx, std::optional<util::cstring_view> id) noexcept override;

		void
		Draw(Graphics& gfx) const noexcept override;

		DirectX::XMMATRIX
		GetTransform() const noexcept override;

		void
		SetRootTransform(DirectX::FXMMATRIX tf) const noexcept;

	private:
		std::string                        fileName;
		std::string                        imguiID;
		std::unique_ptr<Node>              pRoot;
		Node*                              pSelectedNode = nullptr;
		std::vector<std::unique_ptr<Mesh>> allMeshes;
		struct TransformParameters
		{
			float roll  = 0.0f;
			float pitch = 0.0f;
			float yaw   = 0.0f;
			float x     = 0.0f;
			float y     = 0.0f;
			float z     = 0.0f;
		};
		std::unordered_map<size_t, TransformParameters> transforms;
	};
}
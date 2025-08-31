#pragma once
#include "bindings/Bindable.h"
#include "geom/Vertex.h"
#include <vector>

namespace gfx
{
	class DX11Graphics;

	class VertexBuffer : public Bindable<gfx::VertexBuffer, std::string_view, std::string_view>
	{
	public:
		VertexBuffer(
			DX11Graphics&             gfx,
			const geom::VertexBuffer& vbuf,
			std::string_view          fileName,
			std::string_view          tag = ""sv);

		template <class V>
		VertexBuffer(
			DX11Graphics&         gfx,
			const std::vector<V>& vertices,
			std::string_view      fileName,
			std::string_view      tag = ""sv) : Bindable(fileName, tag), stride(sizeof(V))
		{
			D3D11_BUFFER_DESC bd      = {};
			bd.BindFlags              = D3D11_BIND_VERTEX_BUFFER;
			bd.Usage                  = D3D11_USAGE_DEFAULT;
			bd.CPUAccessFlags         = 0u;
			bd.MiscFlags              = 0u;
			bd.ByteWidth              = static_cast<UINT>(sizeof(V) * vertices.size());
			bd.StructureByteStride    = sizeof(V);
			D3D11_SUBRESOURCE_DATA sd = {};
			sd.pSysMem                = vertices.data();
			DX_HR_ERROR_TEST_AND_THROW(GetDevice(gfx)->CreateBuffer(&bd, &sd, &pVertexBuffer));
		}

		void
		Bind(DX11Graphics& gfx) override;

	protected:
		UINT                                 stride;
		Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
	};
}
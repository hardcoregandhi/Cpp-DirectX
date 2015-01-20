#pragma once

#include <directxmath.h>
#include <d3d11_1.h>

using namespace DirectX;

struct SimpleVertex
{
	XMFLOAT3 Pos;
	XMFLOAT3 Normal;
	XMFLOAT2 TexC;

	XMFLOAT3 LightDir;
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;


	bool operator<(const SimpleVertex other) const
	{
		return memcmp((void*)this, (void*)&other, sizeof(SimpleVertex)) > 0;
	};
};

//struct MeshData
//{
//	ID3D11Buffer * VertexBuffer;
//	ID3D11Buffer * IndexBuffer;
//	UINT VBStride;
//	UINT VBOffset;
//	UINT IndexCount;
//};
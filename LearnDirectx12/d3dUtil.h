#pragma once

#include <windows.h>
#include <wrl.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include <string>
#include <memory>
#include <algorithm>
#include <vector>
#include <array>
#include <unordered_map>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <cassert>
#include "d3dx12.h"
#include <pix.h>


class d3dUtil
{
public:
	static UINT CalcConstantBufferByteSize(UINT byteSize)
	{
		return (byteSize + 255) & ~255;
	}


	static Microsoft::WRL::ComPtr<ID3D12Resource> CreateDefaultBuffer(
		ID3D12Device* device, 
		ID3D12GraphicsCommandList* cmdList,
		const void* initData, 
		UINT64 byteSize, 
		Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer);
};

struct SubmeshGeometry
{
	UINT IndexCount = 0;
	UINT StartIndexLocation = 0;
	INT BaseVertexLocation = 0;
	DirectX::BoundingBox Bounds;
};

struct MeshGeometry
{
	std::string Name; // 名称
	Microsoft::WRL::ComPtr<ID3DBlob> VertexBufferCPU = nullptr;
	Microsoft::WRL::ComPtr <ID3DBlob> IndexBufferCPU = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferGPU = nullptr;
	Microsoft::WRL::ComPtr <ID3D12Resource> IndexBufferGPU = nullptr;

	Microsoft::WRL::ComPtr <ID3D12Resource> VertexBufferUploader = nullptr;
	Microsoft::WRL::ComPtr <ID3D12Resource> IndexBufferUploader = nullptr;

	// 缓冲区相关的数据
	UINT VertexByteStride = 0;
	UINT VertexBufferByteSize = 0;
	DXGI_FORMAT IndexFormat = DXGI_FORMAT_R16_UINT;
	UINT IndexBufferByteSize = 0;

	// 一个MeshGeometry结构体能够存储一组顶点/索引缓冲区中多个几何体
	// 若利用下列容器来定义子网格几何体，我们能单独绘制出其中的子网格
	std::unordered_map<std::string, SubmeshGeometry> DrawArgs;

	D3D12_VERTEX_BUFFER_VIEW VertexBufferView()const
	{
		D3D12_VERTEX_BUFFER_VIEW vbv;
		vbv.BufferLocation = VertexBufferGPU->GetGPUVirtualAddress();
		vbv.StrideInBytes = VertexByteStride;
		vbv.SizeInBytes = VertexBufferByteSize;
		return vbv;
	}

	D3D12_INDEX_BUFFER_VIEW IndexBufferView()const
	{
		D3D12_INDEX_BUFFER_VIEW ibv;
		ibv.BufferLocation = IndexBufferGPU->GetGPUVirtualAddress();
		ibv.Format = DXGI_FORMAT_R16_UINT;
		ibv.SizeInBytes = IndexBufferByteSize;
		return ibv;
	}
	// 待数据上传GPU后，我们能释放这些内存
	void DisposeUploaders()
	{
		VertexBufferUploader = nullptr;
		IndexBufferUploader = nullptr;
	}
};

inline std::wstring AnsiToWString(const std::string& str)
{
	WCHAR buffer[512];
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, 512);
	return std::wstring(buffer);
}

class DxException
{
public:
	DxException();
	DxException(HRESULT hr, const std::wstring& functionName, const std::wstring& fileName, int lineNumber);

	std::wstring ToString()const;
public:
	HRESULT ErrorCode = S_OK;
	std::wstring FunctionName;
	std::wstring FileName;
	int LineNumber = -1;
};

#ifndef ThrowIfFailed
#define ThrowIfFailed(x) \
{ \
	HRESULT hr__ = (x);\
	std::wstring wfn = AnsiToWString(__FILE__);\
	if (FAILED(hr__))\
	{ \
		throw DxException(hr__,L#x,wfn,__LINE__);\
	}\
}
#endif

#ifndef ReleaseCom
#define ReleaseCom(x) {if (x){x->Release();x=0;}}
#endif // !ReleaseCom

class d3dUtil
{
};

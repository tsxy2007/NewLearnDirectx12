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
#include "MathHelper.h"

extern const int gNumberFrameResources;

inline std::wstring AnsiToWString(const std::string& str)
{
	WCHAR buffer[512];
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, 512);
	return std::wstring(buffer);
}

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

	static Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(
		const std::wstring& filename,
		const D3D_SHADER_MACRO* defines,
		const std::string& entrypoint,
		const std::string& target
	);

	void LoadFile(const std::string& FilePath);
};

class DxException
{
public:
	DxException() = default;
	DxException(HRESULT hr, const std::wstring& functionName, const std::wstring& filename, int lineNumber);

	std::wstring ToString()const;
	HRESULT ErrorCode = S_OK;
	std::wstring FunctionName;
	std::wstring FileName;
	int LineNumber = -1;
};

struct SubmeshGeometry 
{
	UINT IndexCount = 0;
	UINT StartIndexLocation = 0;
	INT BaseVertexLocation = 0;

	// Bounding box of the geometry defined by this submesh. 
	// This is used in later chapters of the book.
	DirectX::BoundingBox Bounds;
};

struct MeshGeometry
{
	std::string Name;

	Microsoft::WRL::ComPtr<ID3DBlob> VertexBufferCPU = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> IndexBufferCPU = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferGPU = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferGPU = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferUploader = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferUploader = nullptr;

	UINT VertexByteStrider = 0;
	UINT VertexBufferBtyeSize = 0;
	DXGI_FORMAT IndexFormat = DXGI_FORMAT_R16_UINT;
	UINT IndexBufferByteSize = 0;

	std::unordered_map<std::string, SubmeshGeometry> DrawArgs;
public:
	D3D12_VERTEX_BUFFER_VIEW VertexBufferView() const 
	{
		D3D12_VERTEX_BUFFER_VIEW vbv;
		vbv.BufferLocation = VertexBufferGPU->GetGPUVirtualAddress();//vbv 首地址
		vbv.StrideInBytes = VertexByteStrider;//vbv每个元素长度
		vbv.SizeInBytes = VertexBufferBtyeSize; // vbv总共长度
		return vbv;
	}

	D3D12_INDEX_BUFFER_VIEW IndexBufferView() const
	{
		D3D12_INDEX_BUFFER_VIEW ibv;
		ibv.BufferLocation = IndexBufferGPU->GetGPUVirtualAddress();
		ibv.SizeInBytes = IndexBufferByteSize;
		ibv.Format = IndexFormat;
		return ibv;
	}
	void DisposeUploaders()
	{
		VertexBufferUploader = nullptr;
		IndexBufferUploader = nullptr;
	}
};

struct Light
{
	DirectX::XMFLOAT3 Strength = { 0.5f,0.5f,0.5f };
	float FalloffStart = 1.f; // point / spot light only
	DirectX::XMFLOAT3 Direction = { 0.f,-1.f,0.f }; // directional / spot light only
	float FalloffEnd = 10.f; /// point / spot light only
	DirectX::XMFLOAT3 Position = { 0.f,0.f,0.f };// point / spot light only 
	float SpotPower = 64.f; // spot light only 
};

#define MaxLights 16

struct MaterialConstants
{
	DirectX::XMFLOAT4 DiffuseAlbedo = { 1.f,1.f,1.f,1.f };
	DirectX::XMFLOAT3 FresnelRO = { 0.01f,0.01f,0.01f };
	float Roughness = 0.25f;
	DirectX::XMFLOAT4X4 MatTransform = MathHelper::Identity4x4();
};

struct Material
{
	std::string Name;

	int MatCBIndex = -1;
	int DiffuseSrvHeapIndex = -1;
	int NormalSrvHeapIndex = -1;
	int NumFramesDirty = gNumberFrameResources;

	DirectX::XMFLOAT4 DiffuseAlbedo = { 1.f,1.f,1.f,1.f };
	DirectX::XMFLOAT3 FresnelRO = { 0.01f,0.01f,0.01f };
	float Roughness = 0.25f;
	DirectX::XMFLOAT4X4 MatTransform = MathHelper::Identity4x4();
};

struct Texture
{
	std::string Name;
	std::wstring FileName;

	Microsoft::WRL::ComPtr<ID3D12Resource> Resource = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> UploadHeap = nullptr;
};

#ifndef ThrowIfFailed
#define ThrowIfFailed(x) \
{\
	HRESULT hr__ = (x);\
	std::wstring wfn = AnsiToWString(__FILE__);\
	if (FAILED(hr__))\
	{\
		throw DxException(hr__,L#x,wfn,__LINE__);\
	}\
}
#endif

#ifndef ReleaseCom
#define ReleaseCom(x){if(x){x->Release();x=0;}}
#endif
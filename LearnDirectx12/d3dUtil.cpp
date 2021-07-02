#include "d3dUtil.h"
#include <comdef.h>

using Microsoft::WRL::ComPtr;


Microsoft::WRL::ComPtr<ID3D12Resource> d3dUtil::CreateDefaultBuffer(
	ID3D12Device* device,
	ID3D12GraphicsCommandList* cmdList,
	const void* initData,
	UINT64 byteSize,
	Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer)
{
	ComPtr<ID3D12Resource> defaultBuffer;

	CD3DX12_HEAP_PROPERTIES DefaultBufferHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	ThrowIfFailed(device->CreateCommittedResource(
		&DefaultBufferHeap,

	))I;
}

DxException::DxException() = default;

DxException::DxException(HRESULT hr, const std::wstring& functionName, const std::wstring& fileName, int lineNumber)
	:ErrorCode(hr),
	FunctionName(functionName),
	FileName(fileName),
	LineNumber(lineNumber)
{

}

std::wstring DxException::ToString()const
{
	_com_error err(ErrorCode);
	std::wstring msg = err.ErrorMessage();
	return FunctionName + L"failed in" + FileName + L"; line " + std::to_wstring(LineNumber) + L"; error: " + msg;
}
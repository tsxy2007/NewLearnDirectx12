#pragma once

template<typename T>
class UploadBuffer
{
public:
	UploadBuffer(ID3D12Device* device, UINT elementCount, bool isConstantBuffer)
		:mIsConstantBuffer(isConstantBuffer)
	{
		mElementByteSize = sizeof(T);

	}

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> mUploadBuffer;
	BYTE* mMappedData = nullptr;
	UINT mElementByteSize = 0;
	bool mIsConstantBuffer = false;
};


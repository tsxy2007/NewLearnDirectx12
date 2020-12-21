#include "d3dApp.h"
#include "MathHelper.h"
#include "UploadBuffer.h"
#include "GeometryGenerator.h"
#include "FrameResource.h"
#include "Waves.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

const int gNumFrameResources = 3;

struct RenderItem
{
	RenderItem() = default;
	XMFLOAT4X4 World = MathHelper::Identity4x4();
	XMFLOAT4X4 TexTransform = MathHelper::Identity4x4();

	int NumFramesDirty = gNumFrameResources;
	UINT ObjCBIndex = -1;

	Material* Mat = nullptr;
	MeshGeometry* Geo = nullptr;

	D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	UINT IndexCount = 0;
	UINT StartIndexLocation = 0;
	int BaseVertexLocation = 0;
};

enum class RenderLayer : int
{
	Opaque = 0,
	Count,
};

class LitWavesApp : public D3DApp
{
public:
	LitWavesApp(HINSTANCE hInstance);
	LitWavesApp(const LitWavesApp& rhs) = delete;
	LitWavesApp& operator=(const LitWavesApp& rhs) = delete;
	~LitWavesApp();

	virtual bool Initialize() override;
private:
	virtual void OnResize() override;
	virtual void Update(const GameTimer& gt) override;
	virtual void Draw(const GameTimer& gt) override;

	virtual void OnMouseDown(WPARAM btnState, int x, int y) override;
	virtual void OnMouseMove(WPARAM btnState, int x, int y) override;
	virtual void OnMouseUp(WPARAM btnState, int x, int y) override;

	void OnKeyboardInput(const GameTimer& gt);
	void UpdateCamera(const GameTimer& gt);
	void UpdateObjectCBs(const GameTimer& gt);
	void UpdateMaterialCBs(const GameTimer& gt);
	void UpdateMainPassCB(const GameTimer& gt);
	void UpdateWaves(const GameTimer& gt);

	void BuildRootSignature();
	void BuildShadersAndInputLayout();
	void BuildLandGeometry();
	void BuildWavesGeometryBuffers();
	void BuildPSOs();
	void BuildFrameReousrces();
	void BuildRenderItems();
	void DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<RenderItem*>& ritems);

	float GetHillsHeight(float x, float z) const;
	XMFLOAT3 GetHillsNormal(float x, float z)const;

private:
	std::vector<std::unique_ptr<FrameResource>> mFrameResources;
	FrameResource* mCurrFrameResource = nullptr;
	int mCurrFrameResourceIndex = 0;

	UINT mCbvSrvUavDescriptorSize = 0;
	ComPtr<ID3D12RootSignature> mRootSignture = nullptr;

	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> mGeometries;
	std::unordered_map<std::string, std::unique_ptr<Material>> mMaterials;
	std::unordered_map<std::string, std::unique_ptr<Texture>> mTextures;
	std::unordered_map<std::string, std::unique_ptr<ID3DBlob>> mShaders;
	std::unordered_map<std::string, std::unique_ptr<ID3D12PipelineState>> mPSOs;

	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;

	RenderItem* mWavesRitem = nullptr;

	std::vector<std::unique_ptr<RenderItem>> mAllRitems;

	std::vector<RenderItem*> mRitemLayer[(int)RenderLayer::Count];

	std::unique_ptr<Waves> mWaves;

	PassConstants mMainPassCB;

	XMFLOAT3 mEyePos = { 0.f,0.f,0.f };
	XMFLOAT4X4 mView = MathHelper::Identity4x4();
	XMFLOAT4X4 mProj = MathHelper::Identity4x4();

	float mTheta = 1.5f * XM_PI;
	float mPhi = XM_PIDIV2 - 0.1f;
	float mRadius = 50.f;
	float mSunTheta = 1.25f * XM_PI;
	float mSunPhi = XM_PIDIV4;

	POINT mLastMousePos;
};

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	try
	{
		LitWavesApp theApp(hInstance);
		if (!theApp.Initialize())
			return 0;

		return theApp.Run();
	}
	catch (DxException& e)
	{
		MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
		return 0;
	}
}


LitWavesApp::LitWavesApp(HINSTANCE hInstance)
	:D3DApp(hInstance)
{

}

LitWavesApp::~LitWavesApp()
{

}

bool LitWavesApp::Initialize()
{
	if (!D3DApp::Initialize())
	{
		return false;
	}
	return true;
}

void LitWavesApp::OnResize()
{

}

void LitWavesApp::Update(const GameTimer& gt)
{

}

void LitWavesApp::Draw(const GameTimer& gt)
{

}

void LitWavesApp::OnMouseDown(WPARAM btnState, int x, int y)
{

}

void LitWavesApp::OnMouseMove(WPARAM btnState, int x, int y)
{

}

void LitWavesApp::OnMouseUp(WPARAM btnState, int x, int y)
{

}

void LitWavesApp::OnKeyboardInput(const GameTimer& gt)
{

}

void LitWavesApp::UpdateCamera(const GameTimer& gt)
{

}

void LitWavesApp::UpdateObjectCBs(const GameTimer& gt)
{

}

void LitWavesApp::UpdateMaterialCBs(const GameTimer& gt)
{

}

void LitWavesApp::UpdateMainPassCB(const GameTimer& gt)
{

}

void LitWavesApp::UpdateWaves(const GameTimer& gt)
{

}

void LitWavesApp::BuildRootSignature()
{

}

void LitWavesApp::BuildShadersAndInputLayout()
{

}

void LitWavesApp::BuildLandGeometry()
{

}

void LitWavesApp::BuildWavesGeometryBuffers()
{

}

void LitWavesApp::BuildPSOs()
{

}

void LitWavesApp::BuildFrameReousrces()
{

}

void LitWavesApp::BuildRenderItems()
{

}

void LitWavesApp::DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<RenderItem*>& ritems)
{

}

float LitWavesApp::GetHillsHeight(float x, float z) const
{
	return 0.f;
}

DirectX::XMFLOAT3 LitWavesApp::GetHillsNormal(float x, float z) const
{
	return mEyePos;
}

#pragma once

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include "d3dUtil.h"
#include "GameTimer.h"

class D3DApp
{
protected:
	D3DApp(HINSTANCE hInstance);
	D3DApp(const D3DApp& rhs) = delete;
	D3DApp operator=(const D3DApp& rhs) = delete;
	virtual ~D3DApp();
public:
	static D3DApp* GetApp();
	HINSTANCE AppInst() const;
	HWND MainWnd()const;
	float AspectRatio()const;

	bool Get4xMsaaState()const;
	void Set4xMsaaState(bool value);

	int Run();

	virtual bool Initialize();
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
protected:
	virtual void CreateRtvAndDsvDescriptorHeaps();
	virtual void OnResize();
	virtual void Update(const GameTimer& gt) = 0;
	virtual void Draw(const GameTimer& gt) = 0;
	
	// Mouse interface
	virtual void OnMouseDown(WPARAM btnState,int x,int y){}
	virtual void OnMouseUp(WPARAM btnState, int x, int y) {}
	virtual void OnMouseMove(WPARAM btnState, int x, int y) {}
protected:
	// 1. 初始化主窗口
	bool InitMainWindow();
	// 2. 初始化dx12
	bool InitDirect3D();
	// 3. 创建命令列表
	void CreateCommandObjects();
	// 4. 创建交换缓冲
	void CreateSwapChain();
	// flush 命令队列
	void FlushCommandQueue();
	// 
	ID3D12Resource* CurrentBackBuffer()const;
	// 
	D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView()const;
	//
	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView()const;
	//
	void CalculateFrameStats();
	// 
	void LogAdapters();
	//
	void LogAdapterOutputs(IDXGIAdapter* adapter);
	//
	void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);
protected:
	static D3DApp* mApp;

	HINSTANCE mhAppInst = nullptr;
	HWND mhMainWnd = nullptr;
	bool mAppPaused = false;
	bool mMinimized = false;
	bool mMaximized = false;
	bool mResizing = false;
	bool mFullscreenState = false;

	bool m4xMsaaState = false;
	UINT m4xMsaaQuality = 0;

	// 时间管理器
	GameTimer mTimer;

	Microsoft::WRL::ComPtr<IDXGIFactory7> mdxgiFactory; // 工厂
	Microsoft::WRL::ComPtr<IDXGISwapChain> mSwapChain; // 交换
	Microsoft::WRL::ComPtr<ID3D12Device8> md3dDevice; // 硬件


	Microsoft::WRL::ComPtr<ID3D12Fence1> mFence; // fence
	UINT64 mCurrentFence = 0;

	Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCommandQueue;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mDirectCmdListAlloc;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList6> mCommandList;

	static const int SwapChainBufferCount = 2;
	int mCurrBackBuffer = 0;

	Microsoft::WRL::ComPtr<ID3D12Resource2> mSwapChainBuffer[SwapChainBufferCount];
	Microsoft::WRL::ComPtr<ID3D12Resource2> mDepthStencilBuffer;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mRtvHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDsvHeap;

	Microsoft::WRL::ComPtr<ID3D12ProtectedResourceSession> mPSession;


	D3D12_VIEWPORT mScreenViewport;
	D3D12_RECT mScissorRect;

	UINT mRtvDescriptorSize = 0;
	UINT mDsvDescriptorSize = 0;
	UINT mCbvSrvUavDescriptorSize = 0;

	std::wstring mMainWndCaption = L"d3d App";
	D3D_DRIVER_TYPE md3dDriverType = D3D_DRIVER_TYPE_HARDWARE;
	DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	int mClientWidth = 640;
	int mClientHeight = 400;
};
#include "d3dUtil.h"
#include "D3DApp.h"

extern LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"MainWnd";
	if (!RegisterClass(&wc))
	{
		MessageBox(NULL, L"RegisterClass Failed.", 0, 0);
		return false;
	}

	using Microsoft::WRL::ComPtr;
	LONG mClientWidth = 800;
	LONG mClientHeight = 800;
	RECT R = { 0,0,mClientWidth,mClientHeight };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width = R.right - R.left;
	int height = R.bottom - R.top;
	std::wstring mMainWndCaption = L"d3d App";
	HWND mhMainWnd = CreateWindow(L"MainWnd", mMainWndCaption.c_str(),
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, hInstance, 0);
	if (!mhMainWnd)
	{
		MessageBox(0, L"CreateWindow Failed.", 0, 0);
		return false;
	}

	Microsoft::WRL::ComPtr<IDXGIFactory7> Factory;
	CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(Factory.GetAddressOf()));
	
	IDXGIAdapter1* Adapter = nullptr;
	std::vector<IDXGIAdapter1*> adapterList;
	int i = 0;
	while (Factory->EnumAdapters1(i, &Adapter) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_ADAPTER_DESC1 desc;
		Adapter->GetDesc1(&desc);
		std::wstring text = L"**Adataper:";
		text += desc.Description;
		text += L"\n";
		OutputDebugString(text.c_str() );
		adapterList.push_back(Adapter);
		i++;
	}
	;
	DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	for (int i = 0 ;i<adapterList.size();i++)
	{
		IDXGIAdapter1* adapter = adapterList[i];
		IDXGIOutput* output = nullptr;
		int j = 0;
		while (adapter->EnumOutputs(j,&output) != DXGI_ERROR_NOT_FOUND)
		{
			DXGI_OUTPUT_DESC desc;
			output->GetDesc(&desc);

			std::wstring text = L"** Ouput:";
			text += desc.DeviceName;
			text += L"\n";
			OutputDebugString(text.c_str());
			++j;


			UINT count = 0;
			UINT flags = 0;
			output->GetDisplayModeList(mBackBufferFormat, flags, &count, nullptr);
			std::vector<DXGI_MODE_DESC> modelist(count);
			output->GetDisplayModeList(mBackBufferFormat, flags, &count, &modelist[0]);
			for (const auto& x : modelist)
			{
				UINT n = x.RefreshRate.Numerator;
				UINT d = x.RefreshRate.Denominator;
				std::wstring text =
					L"Width = " + std::to_wstring(x.Width) + L" " +
					L"Height = " + std::to_wstring(x.Height) + L" " +
					L"Refresh = " + std::to_wstring(n) + L"/" + std::to_wstring(d) +
					L"\n";

				::OutputDebugString(text.c_str());
			}
		}
	}
	// 启动debug层;
	{
		ComPtr<ID3D12Debug> debugController;
		D3D12GetDebugInterface(IID_PPV_ARGS(&debugController));
		debugController->EnableDebugLayer();
	}
	ID3D12Device2* Device = nullptr;

	HRESULT hardwareResult = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&Device));

	//创建device
	if (hardwareResult == S_FALSE)
	{
		Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;
		Factory->EnumWarpAdapter(IID_PPV_ARGS(&adapter));
		D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&Device));
	}

	if (Device == nullptr)
	{
		return false;
	}

	//创建Fence
	{
		ID3D12Fence1* Fence = nullptr;
		Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&Fence));
		
		UINT RTVSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		UINT DSVSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		UINT CBVSRVUAVSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		UINT i = 0;
	

	//检测GPU是否对MSAA质量级别的支持
	
		D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLvels;
		msQualityLvels.Format = mBackBufferFormat;
		msQualityLvels.SampleCount = 4;
		msQualityLvels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
		msQualityLvels.NumQualityLevels = 0;

		Device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &msQualityLvels,sizeof(msQualityLvels));

	

	//创建渲染命令队列；命令列表；命令分配器
	
		//1:创建渲染队列
		ComPtr<ID3D12CommandQueue> CommandQueue;
		D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {};
		commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		Device->CreateCommandQueue(&commandQueueDesc,IID_PPV_ARGS(&CommandQueue));

		//2:创建渲染命令分配器
		ComPtr<ID3D12CommandAllocator> CommandAllocator = {};
		Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,IID_PPV_ARGS(&CommandAllocator));

		//3:创建渲染命令列表
		ComPtr<ID3D12GraphicsCommandList2> CommandList = {};
		Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, CommandAllocator.Get(), nullptr, IID_PPV_ARGS(CommandList.GetAddressOf()));
		CommandList->Close();

	// 描述并创建交换链
		static const int SwapChainBufferCount = 2;
		ComPtr<IDXGISwapChain> SwapChain;
		SwapChain.Reset();
		DXGI_SWAP_CHAIN_DESC sd;
		sd.BufferDesc.Width = mClientWidth;
		sd.BufferDesc.Height = mClientHeight;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferDesc.Format = mBackBufferFormat;
		sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		sd.SampleDesc.Count = 1; // 多重采样
		sd.SampleDesc.Quality = 0;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.BufferCount = SwapChainBufferCount;
		sd.OutputWindow = mhMainWnd;
		sd.Windowed = true;
		sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		Factory->CreateSwapChain(CommandQueue.Get(),
			&sd, 
			SwapChain.GetAddressOf());
		

		// 创建描述符堆
		ComPtr<ID3D12DescriptorHeap> rtvDescriptor;
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
		rtvHeapDesc.NumDescriptors = SwapChainBufferCount;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		rtvHeapDesc.NodeMask = 0;

		Device->CreateDescriptorHeap(&rtvHeapDesc, 
			IID_PPV_ARGS(rtvDescriptor.GetAddressOf()));

		ComPtr<ID3D12DescriptorHeap> dsvHeap;
		D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
		dsvHeapDesc.NumDescriptors = 1;
		dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		dsvHeapDesc.NodeMask = 0;
		Device->CreateDescriptorHeap(&dsvHeapDesc, 
			IID_PPV_ARGS(dsvHeap.GetAddressOf()));

		// 创建渲染目标视图
		
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(
			rtvDescriptor->GetCPUDescriptorHandleForHeapStart()
		);
		ComPtr<ID3D12Resource> Swapchainbuffer[SwapChainBufferCount];
		for (UINT i = 0; i < SwapChainBufferCount; i++)
		{
			//1.获得存于交换链中的缓冲区资源
			SwapChain->GetBuffer(SwapChainBufferCount, IID_PPV_ARGS(&Swapchainbuffer[i]));
		    //2.创建RTV
			Device->CreateRenderTargetView(Swapchainbuffer[i].Get(), nullptr, rtvHeapHandle);
			//3.偏移到描述符堆中的下一个缓冲区
			rtvHeapHandle.Offset(1, RTVSize);
		}

		// 创建深度/模板缓冲区及其视图
		//1.通过填写D3D12_RESOURCE_DESC结构体描述纹理资源
		ComPtr<ID3D12Resource> DepthStencilResource = nullptr;
		DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_R24G8_TYPELESS;

		D3D12_RESOURCE_DESC depthStencilDesc;
		depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		depthStencilDesc.Alignment = 0;
		depthStencilDesc.Width = mClientWidth;
		depthStencilDesc.Height = mClientHeight;
		depthStencilDesc.DepthOrArraySize = 1;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.Format = mDepthStencilFormat;
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
		depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;


		D3D12_CLEAR_VALUE  optClear;
		optClear.Format = mDepthStencilFormat;
		optClear.DepthStencil.Depth = 1.f;
		optClear.DepthStencil.Stencil = 0;
		//2.再用ID3D12Device::CreateCommittedResource
		Device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&depthStencilDesc,
			D3D12_RESOURCE_STATE_COMMON,
			&optClear,
			IID_PPV_ARGS(DepthStencilResource.GetAddressOf())
		);
		//利用此资源的格式，为整个资源的第0mip层创建描述符；
		Device->CreateDepthStencilView(
			DepthStencilResource.Get(),
			nullptr,
			dsvHeap->GetCPUDescriptorHandleForHeapStart()
		);
		// 将资源从初始状态转换为深度缓冲区
		CommandList->ResourceBarrier(
			1,
			&CD3DX12_RESOURCE_BARRIER::Transition(
				DepthStencilResource.Get(),
				D3D12_RESOURCE_STATE_COMMON,
				D3D12_RESOURCE_STATE_DEPTH_WRITE
			)
		);

		// 设置视口
		D3D12_VIEWPORT vp;
		vp.TopLeftX = 0.f;
		vp.TopLeftY = 0.f;
		vp.Width = static_cast<float>(mClientWidth);
		vp.Height = static_cast<float>(mClientHeight);
		vp.MinDepth = 0.f;
		vp.MaxDepth = 1.f;
		CommandList->RSSetViewports(1, &vp);

		// 设置裁剪矩形
		D3D12_RECT mScissorRect = { 0,0,mClientWidth / 2,mClientHeight / 2 };
		CommandList->RSSetScissorRects(1, &mScissorRect);
	}
	ShowWindow(mhMainWnd, SW_SHOW);
	UpdateWindow(mhMainWnd);
	
	return true;
}

#include "D3DApp.h"

using Microsoft::WRL::ComPtr;
using namespace std;
using namespace DirectX;


LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (D3DApp::GetApp() != nullptr)
	{
		return D3DApp::GetApp()->MsgProc(hwnd, msg, wParam, lParam);
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

D3DApp* D3DApp::mApp = nullptr;

D3DApp* D3DApp::GetApp()
{
	return mApp;
}

D3DApp::D3DApp(HINSTANCE hInstance)
	:mhAppInst(hInstance)
{
	assert(mApp == nullptr);
	mApp = this;
}

D3DApp::~D3DApp()
{
	if (md3dDevice != nullptr)
	{
		FlushCommandQueue();
	}
}

HINSTANCE D3DApp::AppInst() const 
{
	return mhAppInst;
}

HWND  D3DApp::MainWnd() const
{
	return mhMainWnd;
}
float D3DApp::AspectRatio() const
{
	return static_cast<float>(mClientWidth)/mClientHeight;
}

bool D3DApp::Get4xMsaaState() const
{
	return m4xMsaaState;
}

void D3DApp::Set4xMsaaState(bool value)
{
	if (m4xMsaaState != value)
	{
		m4xMsaaState = value;
		CreateSwapChain();
		OnResize();
	}
}

int D3DApp::Run()
{

	return 0;
}

bool D3DApp::Initialize()
{
	if (!InitMainWindow())
	{
		return false;
	}
	if (!InitDirect3D())
	{
		return false;
	}
	return true;
}

LRESULT D3DApp::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

void D3DApp::CreateRtvAndDsvDescriptorHeaps()
{


}

void D3DApp::OnResize( )
{

}

bool D3DApp::InitMainWindow()
{
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = mhAppInst;
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
	return true;
}

bool D3DApp::InitDirect3D()
{
#if defined (DEBUG) || defined(_DEBUG)
	{
		ComPtr<ID3D12Debug> debugController;
		ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
		debugController->EnableDebugLayer();
	}
#endif
	return true;
}

void D3DApp::CreateCommandObjects()
{

}

void D3DApp::CreateSwapChain()
{

}

void D3DApp::FlushCommandQueue()
{

}

D3D12_CPU_DESCRIPTOR_HANDLE D3DApp::DepthStencilView() const
{
	return mDsvHeap->GetCPUDescriptorHandleForHeapStart();
}

void D3DApp::CalculateFrameStats()
{

}
void D3DApp::LogAdapters()
{

}

void D3DApp::LogAdapterOutputs(IDXGIAdapter* adapter)
{

}
void D3DApp::LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format)
{

}
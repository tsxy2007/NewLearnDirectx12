#include <windows.h>
#include <xstring>
#include "d3dx12.h"
#include <wrl.h>
#include "dxgi1_6.h"
#include "vector"
#include <iostream>

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hwnd, msg, wParam, lParam);
}
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

	RECT R = { 0,0,800,800 };
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

	ShowWindow(mhMainWnd, SW_SHOW);
	UpdateWindow(mhMainWnd);
	
	return true;
}

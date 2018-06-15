#pragma once
#pragma once
#include <d3dx9.h>
#include <d3d9.h>
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "d3d9.lib")

//DirectX stuff
IDirect3D9Ex* D3DObject = 0;
IDirect3DDevice9Ex* D3DDevice = 0;
D3DPRESENT_PARAMETERS Parameters;
RECT rc;
ID3DXLine* Line;
ID3DXFont* FontSmall;

//WinMain Window
int width = 1920; // We can use GetSystemMetrics() here too
int height = 1080;

//Overlay
LPCWSTR OVWindowName = L"Overlay";
HWND OVWnd;

//Target Window
char TargetWindowName[256] = "IW4x: Multiplayer";
HWND TargetWnd;
RECT TargetSize;

MSG Message;

const MARGINS Margin = { 0, 0, width, height };


void DrawOverlayText(char* string, int x, int y, int a, int r, int g, int b, ID3DXFont* font)
{
	RECT Font;
	Font.left = x;
	Font.top = y;
	font->DrawTextA(0, string, strlen(string), &Font, DT_NOCLIP, D3DCOLOR_ARGB(a, r, g, b));
}

void DrawOverlayRectangle(float x, float y, float w, float h, int a, int r, int g, int b)
{
	D3DCOLOR color = D3DCOLOR_ARGB(a, r, g, b);
	D3DRECT rect = { x, y, w, h };
	D3DDevice->Clear(1, &rect, D3DCLEAR_TARGET | D3DCLEAR_TARGET, color, 0, 0);
}

int InitDirectX(HWND hWnd)
{
	if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &D3DObject)))
		exit(1);

	ZeroMemory(&Parameters, sizeof(Parameters));
	Parameters.Windowed = TRUE;
	Parameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	Parameters.hDeviceWindow = hWnd;
	Parameters.MultiSampleQuality = D3DMULTISAMPLE_NONE;
	Parameters.BackBufferFormat = D3DFMT_A8R8G8B8;
	Parameters.BackBufferWidth = width;
	Parameters.BackBufferHeight = height;
	Parameters.EnableAutoDepthStencil = TRUE;
	Parameters.AutoDepthStencilFormat = D3DFMT_D16;

	if (FAILED(D3DObject->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &Parameters, 0, &D3DDevice)))
		exit(1);

	return 0;
}

int RenderOverlay()
{
	if (TargetWnd == GetForegroundWindow())
	{
		char * value = TargetWindowName;

		HWND newhwnd = FindWindowA(NULL, value);
		if (newhwnd != NULL) {
			GetWindowRect(newhwnd, &rc);
		}
		else {
			ExitProcess(0);
		}

		D3DDevice->Clear(0, 0, D3DCLEAR_TARGET, 0, 1.0f, 0);
		D3DDevice->BeginScene();

		//////////////////Draw Overlay here////////////////////

		//left
		DrawOverlayRectangle(954, 539, 959, 541, 250, 255, 0, 0);
		//top
		DrawOverlayRectangle(959, 534, 961, 539, 250, 255, 0, 0);
		//right
		DrawOverlayRectangle(961, 539, 966, 541, 250, 255, 0, 0);
		//bottom
		DrawOverlayRectangle(959, 541, 961, 546, 250, 255, 0, 0);

		D3DDevice->EndScene();
		D3DDevice->PresentEx(0, 0, 0, 0, 0);
		return 0;
	}
}

LRESULT CALLBACK WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch (Message)
	{
	case WM_PAINT:
		RenderOverlay();
		break;

	case WM_CREATE:
		DwmExtendFrameIntoClientArea(hWnd, &Margin);
		break;

	case WM_DESTROY:
		PostQuitMessage(1);
		return 0;

	default:
		return DefWindowProc(hWnd, Message, wParam, lParam);
		break;
	}
	return 0;
}

void SetWindowToTarget()
{
	while (true)
	{
		TargetWnd = FindWindowA(0, TargetWindowName);
		if (TargetWnd)
		{
			GetWindowRect(TargetWnd, &TargetSize);
			width = TargetSize.right - TargetSize.left;
			height = TargetSize.bottom - TargetSize.top;
			DWORD dwStyle = GetWindowLong(TargetWnd, GWL_STYLE);
			MoveWindow(OVWnd, TargetSize.left, TargetSize.top, width, height, true);
		}

		Sleep(1);
	}
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hSecInstance, LPSTR nCmdLine, INT nCmdShow)
{

	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)SetWindowToTarget, 0, 0, 0);

	WNDCLASSEX wClass;
	wClass.cbClsExtra = NULL;
	wClass.cbSize = sizeof(WNDCLASSEX);
	wClass.cbWndExtra = NULL;
	wClass.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(0, 0, 0));
	wClass.hCursor = LoadCursor(0, IDC_ARROW);
	wClass.hIcon = LoadIcon(0, IDI_APPLICATION);
	wClass.hIconSm = LoadIcon(0, IDI_APPLICATION);
	wClass.hInstance = hInstance;
	wClass.lpfnWndProc = WinProc;
	wClass.lpszClassName = OVWindowName;
	wClass.lpszMenuName = OVWindowName;
	wClass.style = CS_VREDRAW | CS_HREDRAW;

	if (!RegisterClassEx(&wClass))
		exit(1);

	TargetWnd = FindWindowA(0, TargetWindowName);
	if (TargetWnd)
	{
		GetWindowRect(TargetWnd, &TargetSize);
		width = TargetSize.right - TargetSize.left;
		height = TargetSize.bottom - TargetSize.top;
		OVWnd = CreateWindowEx(WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED, OVWindowName, OVWindowName, WS_POPUP, 690, 540, width, height, 0, 0, 0, 0);
		SetLayeredWindowAttributes(OVWnd, 0, 1.0f, LWA_ALPHA);
		SetLayeredWindowAttributes(OVWnd, 0, RGB(0, 0, 0), LWA_COLORKEY);
		ShowWindow(OVWnd, SW_SHOW);
	}

	InitDirectX(OVWnd);

	for (;;)
	{
		if (PeekMessage(&Message, OVWnd, 0, 0, PM_REMOVE))
		{
			DispatchMessage(&Message);
			TranslateMessage(&Message);
		}
		Sleep(1);
	}
	return 0;
}
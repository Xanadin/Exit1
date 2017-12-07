#include "StdInclude.h"
#include "App.h"

Window::Window(LPCTCH name, int clientWidth, int clientHeight)
{
	_tcscpy_s(Caption, MAXNAMELEN, name);
	if (clientWidth && clientHeight)
	{ 
		mClientWidth = clientWidth;
		mClientHeight = clientHeight;
	}
	else
	{

	}
}

bool Window::InitMainWindow()
{
	// Get process handle
	HINSTANCE hProcess = GetModuleHandle(NULL);
	// Register window class
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hProcess;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = Caption;

	if (!RegisterClass(&wc))
	{
		MessageBox(0, TEXT("RegisterClass Failed."), 0, 0);
		return false;
	}
	// Calc total window size based on client size
	RECT rWindow = { 0, 0, mClientWidth, mClientHeight };
	AdjustWindowRect(&rWindow, WS_OVERLAPPEDWINDOW, false);
	int width = rWindow.right - rWindow.left;
	int height = rWindow.bottom - rWindow.top;
	// Create window object
	hWindow = CreateWindow(
		Caption,
		Caption,
		WS_OVERLAPPEDWINDOW, 
		CW_USEDEFAULT, 
		CW_USEDEFAULT, 
		width, 
		height, 
		0, 
		0, 
		hProcess,
		(LPVOID)this);

	if (!hWindow)
	{
		MessageBox(0, TEXT("CreateWindow Failed."), 0, 0);
		return false;
	}
	// Center window
	HWND hDesktop = GetDesktopWindow();
	RECT rDesktop;
	GetWindowRect(hDesktop, &rDesktop);
	SetWindowPos(hWindow, HWND_TOP,
		(rDesktop.right - width) / 2,
		(rDesktop.bottom - height) / 2,
		width,
		height,
		SWP_NOZORDER | SWP_SHOWWINDOW);
	UpdateWindow(hWindow);
	return true;
}


LRESULT Window::MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Window* pWindow = NULL;

	// what is the message 
	if (msg == WM_NCCREATE)
	{
		// Alla creazione della finestra memorizza un puntatore ad essa in una struttura dati interna
		pWindow = reinterpret_cast<Window*>(((LPCREATESTRUCT)lParam)->lpCreateParams);
		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindow));
	}

	else
		// Rintraccia il puntatore alla finestra che sta chiamando la funzione e passa i messaggi alla procedura di questa finestra
		pWindow = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	if (pWindow != NULL)
	{
		return pWindow->MsgProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

LRESULT Window::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			mAppPaused = true;
			mTimer.Stop();
		}
		else
		{
			mAppPaused = false;
			mTimer.Start();
		}
		return 0;

	case WM_SIZE:
		mClientWidth = LOWORD(lParam);
		mClientHeight = HIWORD(lParam);
		if (mD3DDevice)
		{
			if (wParam == SIZE_MINIMIZED)
			{
				mAppPaused = true;
				mMinimized = true;
				mMaximized = false;
			}
			else if (wParam == SIZE_MAXIMIZED)
			{
				mAppPaused = false;
				mMinimized = false;
				mMaximized = true;
				OnResize();
			}
			else if (wParam == SIZE_RESTORED)
			{
				if (mMinimized)
				{
					mAppPaused = false;
					mMinimized = false;
					OnResize();
				}
				else if (mMaximized)
				{
					mAppPaused = false;
					mMaximized = false;
					OnResize();
				}
				else if (mResizing)
				{
					// If user is dragging the resize bars, we do not resize 
					// the buffers here because as the user continuously 
					// drags the resize bars, a stream of WM_SIZE messages are
					// sent to the window, and it would be pointless (and slow)
					// to resize for each WM_SIZE message received from dragging
					// the resize bars.  So instead, we reset after the user is 
					// done resizing the window and releases the resize bars, which 
					// sends a WM_EXITSIZEMOVE message.

				}
				else // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
				{
					OnResize();
				}
			}
		}
		return 0;

	case WM_ENTERSIZEMOVE:
		mAppPaused = true;
		mResizing = true;
		mTimer.Stop();
		return 0;

	case WM_EXITSIZEMOVE:
		mAppPaused = false;
		mResizing = false;
		mTimer.Start();
		OnResize();
		return 0;

		// Catch this message so to prevent the window from becoming too small.
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
		return 0;

	case WM_CLOSE:
		DestroyWindow(hwnd);
		return 0;


	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

		// The WM_MENUCHAR message is sent when a menu is active and the user presses 
		// a key that does not correspond to any mnemonic or accelerator key. 
	case WM_MENUCHAR:
		// Don't beep when we alt-enter.
		return MAKELRESULT(0, MNC_CLOSE);

	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		OnMouseDown(wParam, ((int)(short)LOWORD(lParam)), ((int)(short)HIWORD(lParam)));
		return 0;

	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		OnMouseUp(wParam, ((int)(short)LOWORD(lParam)), ((int)(short)HIWORD(lParam)));
		return 0;

	case WM_MOUSEMOVE:
		OnMouseMove(wParam, ((int)(short)LOWORD(lParam)), ((int)(short)HIWORD(lParam)));
		return 0;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

bool Renderer::InitDirect3D()
{
	// Create Device

	UINT createDeviceFlags = 0;

	// If the project is in a debug build, enable debugging via SDK Layers
#if defined(DEBUG) || defined(_DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	return false;
}

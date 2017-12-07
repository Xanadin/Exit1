#pragma once

class Window
{
public:
	Window(LPCTCH name, int clientWidth = 0, int clientHeight = 0);
private:
	bool InitMainWindow();
	HWND hWindow;
	int mClientWidth;
	int mClientHeight;
	TCHAR Caption[MAXNAMELEN];
	LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	// FSM
	bool mAppPaused = false;
	bool mMinimized = false;
	bool mMaximized = false;
	bool mResizing = false;
	const int DefaultClientWidth = 800;
	const int DefaultClientHeight = 600;
};



class Renderer
{
	bool InitDirect3D();
};

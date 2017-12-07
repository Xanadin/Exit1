
#include <vector>
#include <utility>
#include <map>

#include "StdInclude.h"

#include "App.h"

BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
	OutputDebugString(TEXT("Monitor Count Called"));
	int *Count = (int*)dwData;
	(*Count)++;
	return TRUE;
}

BOOL CALLBACK MonitorInfoProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM pData)
{
	OutputDebugString(TEXT("Monitor Info Called"));
	std::vector<MONITORINFOEX>* pInfo = (std::vector<MONITORINFOEX>*)pData;
	static int count = 0;
	MONITORINFOEX info;
	ZeroMemory(&info, sizeof(MONITORINFOEX));
	info.cbSize = sizeof(MONITORINFOEX);
	GetMonitorInfo(hMonitor, &info);
	pInfo->push_back(info);
	count++;
	return TRUE;
}


int MonitorCount()
{
	int Count = 0;
	
	std::vector<MONITORINFOEX> info;
	EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, (LPARAM)&Count);
	info.resize(Count);
	EnumDisplayMonitors(NULL, NULL, MonitorInfoProc, (LPARAM)&info);
	// Ritorna anche dei device che non sono monitor. Usare DXGI.
	return -1;//signals an error
}

class System
{
public:
	System()
	{
		Windows7SP1OrGreater = IsWindows7SP1OrGreater();
		MonitorNum = GetSystemMetrics(SM_CMONITORS);
		PrimaryDisplayScreenWidth = GetSystemMetrics(SM_CXSCREEN);
		PrimaryDisplayScreenHeight = GetSystemMetrics(SM_CYSCREEN);
		ClientWidth = GetSystemMetrics(SM_CXFULLSCREEN);
		ClientHeight = GetSystemMetrics(SM_CYFULLSCREEN);
		MonitorCount();
		MouseButtonsNum = GetSystemMetrics(SM_CMOUSEBUTTONS);
	}
	// Get OS info
	// IsWindows >= Windows7 service pack 1
	// Get Number of Monitors
	// Get Desk
	// Get CPU info
	// Get Disk info
	// Get RAM info
	// Get GPU info
private:
	int MouseButtonsNum = 0; // 0 = No mouse
	int MonitorNum = 0;
	int PrimaryDisplayScreenWidth = 0;
	int PrimaryDisplayScreenHeight = 0;
	int ClientWidth = 0;
	int ClientHeight = 0;
	bool Windows7SP1OrGreater = false;
};


int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR pszCmdLine, int nCmdShow)
{
	// Get system info
	System sys;
	// Check minimum requisites
		// Launch config window
		// Save prefs
	// Create Window/s
		// Check prefs
	// Initialize Subsistems
		// Initialize DirectX
		// ....
	// Load assets
	// Launch app
		// If select start Enter Loop
			// Update timer
			// Poll Inputs
			// Advance Logic
			// Render frame
			// Present if render complete
	return 0;
}

class Renderer
{

};



/**
* Filename: CefWebBrowser.cpp
* Author: Owsap
**/

#include "StdAfx.h"

#if defined(ENABLE_CEF_BROWSER)
#include "CefWebBrowser.h"

#include "cef/cef_app.h"
#include "cef/cef_browser.h"

constexpr const char* WEBBROWSER_CLASSNAME = "WEBBROWSER";
constexpr const char* WEBBROWSER_WINDOW_TEXT = "m2CefBrowser";

static HINSTANCE gs_hInstance = NULL;
static HWND gs_hWndParent = NULL;

static CefRefPtr<ClientApp> gs_pkClientApp;
static ClientHandler* gs_pClientHandle = NULL;

using CefWebBrowserMap = std::map<BYTE, HWND>;
static CefWebBrowserMap gs_hWndCefWebBrowserMap;

void CleanupCefWindows()
{
	for (auto& pair : gs_hWndCefWebBrowserMap)
	{
		HWND hWnd = pair.second;
		if (hWnd != NULL)
		{
			DestroyWindow(hWnd);
		}
	}
	gs_hWndCefWebBrowserMap.clear();

	if (gs_hInstance)
		UnregisterClass(WEBBROWSER_CLASSNAME, gs_hInstance);
}

LRESULT CALLBACK CefWebBrowserWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		//case WM_CLOSE:
		//{
		//	CleanupCefWindows();
		//	// Handle other cleanup tasks or exit the application.
		//	// For example: PostQuitMessage(0);
		//	return 0;
		//}
		//case WM_DESTROY:
		//	return(TRUE);
		case WM_SIZE:
			return(FALSE);
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

CefWebBrowser::CefWebBrowser()
{
	gs_hWndCefWebBrowserMap.clear();
}

CefWebBrowser::~CefWebBrowser()
{
	Destroy();
}

bool CefWebBrowser::Show(const BYTE c_bType, HWND hParent, const char* c_szURL, const RECT* c_rcWebPage)
{
	CefWebBrowserMap::iterator it = gs_hWndCefWebBrowserMap.find(c_bType);
	if ((it != gs_hWndCefWebBrowserMap.end()) && it->second)
		return false;

	gs_hWndParent = hParent;

	HWND hWndWebBrowser = CreateWindowEx(0, WEBBROWSER_CLASSNAME, WEBBROWSER_WINDOW_TEXT, WS_CHILD | WS_VISIBLE,
		c_rcWebPage->left, c_rcWebPage->top, c_rcWebPage->right - c_rcWebPage->left, c_rcWebPage->bottom - c_rcWebPage->top,
		hParent, NULL, gs_hInstance, 0);

	if (hWndWebBrowser == NULL)
		return false;

	gs_hWndCefWebBrowserMap[c_bType] = hWndWebBrowser;

	RECT rc;
	GetClientRect(hWndWebBrowser, &rc);
	
	CefWindowInfo info;
	info.SetAsChild(hWndWebBrowser, rc);
	SetWindowText(hWndWebBrowser, WEBBROWSER_WINDOW_TEXT);
	
	CefRefPtr<CefClient> client(new ClientHandler);
	
	CefBrowserSettings browser_settings;
	CefBrowserHost::CreateBrowser(info, client.get(), c_szURL, browser_settings, NULL, NULL);

	ShowWindow(hWndWebBrowser, SW_SHOW);
	UpdateWindow(hWndWebBrowser);

	SetFocus(hWndWebBrowser);

	return true;
}

void CefWebBrowser::Move(const BYTE c_bType, const RECT* c_rcWebPage)
{
	CefWebBrowserMap::iterator it = gs_hWndCefWebBrowserMap.find(c_bType);
	if (it != gs_hWndCefWebBrowserMap.end())
	{
		MoveWindow(it->second, c_rcWebPage->left, c_rcWebPage->top, c_rcWebPage->right - c_rcWebPage->left, c_rcWebPage->bottom - c_rcWebPage->top, 1);
		SetFocus(it->second);
	}
}

void CefWebBrowser::Hide(const BYTE c_bType)
{
	CefWebBrowserMap::iterator it = gs_hWndCefWebBrowserMap.find(c_bType);
	if (it != gs_hWndCefWebBrowserMap.end())
	{
		HWND hWnd = it->second;
		if (hWnd == NULL)
			return;

		ShowWindow(hWnd, SW_HIDE);

		if (IsWindow(hWnd))
			DestroyWindow(hWnd);

		hWnd = NULL;
		gs_hWndCefWebBrowserMap.erase(it);
	}

	if (gs_hWndParent)
		SetFocus(gs_hWndParent);
}

bool CefWebBrowser::IsVisible(const BYTE c_bType)
{
	CefWebBrowserMap::iterator it = gs_hWndCefWebBrowserMap.find(c_bType);
	if (it != gs_hWndCefWebBrowserMap.end())
		return (it->second != NULL);
	return false;
}

bool CefWebBrowser::IsAnyVisible()
{
	CefWebBrowserMap::iterator it = gs_hWndCefWebBrowserMap.begin();
	for (; it != gs_hWndCefWebBrowserMap.end(); ++it)
		if (it->second != NULL)
			return true;
	return false;
}

void CefWebBrowser::Destroy()
{
	for (const auto& it : gs_hWndCefWebBrowserMap)
		Hide(it.first);

	Cleanup();
}

bool CefWebBrowser::Startup(HINSTANCE hInstance)
{
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEX));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hInstance = hInstance;
	wc.lpfnWndProc = CefWebBrowserWindowProc;
	wc.lpszClassName = WEBBROWSER_CLASSNAME;
	//wc.lpszMenuName = WEBBROWSER_WINDOW_TEXT;
	RegisterClassEx(&wc);

	gs_hInstance = hInstance;

	gs_pkClientApp = new ClientApp;
	if (gs_pkClientApp == NULL)
		return false;

	CefSettings settings;
	settings.no_sandbox = true;
	settings.multi_threaded_message_loop = true;
#if defined(_DEBUG)
	settings.log_severity = LOGSEVERITY_WARNING;
#else
	settings.log_severity = LOGSEVERITY_DISABLE;
#endif
	settings.ignore_certificate_errors = true;

	std::filesystem::path temp_dir_path = std::filesystem::temp_directory_path();
	std::filesystem::path cache_path = temp_dir_path / "m2CefBrowser";
	std::string cache_path_string = cache_path.string();
	CefString(&settings.cache_path).FromASCII(cache_path_string.c_str());

	CefMainArgs main_args(gs_hInstance);
	if (!CefInitialize(main_args, settings, gs_pkClientApp.get(), NULL))
		return false;

	return true;
}

void CefWebBrowser::Cleanup()
{
	if (gs_hInstance)
		UnregisterClass(WEBBROWSER_CLASSNAME, gs_hInstance);

	CefShutdown();
}
#endif

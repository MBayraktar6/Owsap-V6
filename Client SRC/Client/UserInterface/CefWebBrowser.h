/**
* Filename: CefWebBrowser.h
* Author: Owsap
**/

#pragma once

#if defined(ENABLE_CEF_BROWSER)
#include "CefClientApp.h"
#include "CefClientHandler.h"

class CefWebBrowser : public CSingleton<CefWebBrowser>
{
public:
	enum EBrowserType
	{
		BROWSER_TYPE_ITEM_MALL,
		BROWSER_TYPE_DUTCH,
		BROWSER_TYPE_POPUP_NOTICE,

		BROWSER_TYPE_MAX,
	};

	CefWebBrowser();
	~CefWebBrowser();

	static bool Startup(HINSTANCE hInstance);
	static void Destroy();
	static void Cleanup();

	static bool Show(const BYTE c_bType, HWND parent, const char* addr, const RECT* c_rcWebPage);
	static void Hide(const BYTE c_bType);

	static void Move(const BYTE c_bType, const RECT* c_rcWebPage);

	static bool IsVisible(const BYTE c_bType);
	static bool IsAnyVisible();
};
#endif

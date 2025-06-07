#include "StdAfx.h"
#include "PythonApplication.h"
#include "CefWebBrowser.h"
#include "../CWebBrowser/CWebBrowser.h"

bool CPythonApplication::IsWebPageMode(const BYTE c_bType)
{
#if defined(ENABLE_CEF_BROWSER)
	return CefWebBrowser::IsVisible(c_bType);
#else
	return WebBrowser_IsVisible() ? true : false;
#endif
}

void CPythonApplication::ShowWebPage(const BYTE c_bType, const char* c_szURL, const RECT& c_rcWebPage)
{
#if defined(ENABLE_CEF_BROWSER)
	if (CefWebBrowser::IsVisible(c_bType))
		return;

	m_grpDevice.EnableWebBrowserMode(c_rcWebPage);
	if (!CefWebBrowser::Show(c_bType, GetWindowHandle(), c_szURL, &c_rcWebPage))
		TraceError("CREATE_WEBBROWSER_ERROR:%d", GetLastError());
#else
	if (WebBrowser_IsVisible())
		return;

	m_grpDevice.EnableWebBrowserMode(c_rcWebPage);
	if (!WebBrowser_Show(GetWindowHandle(), c_szURL, &c_rcWebPage))
		TraceError("CREATE_WEBBROWSER_ERROR:%d", GetLastError());
#endif

	SetCursorMode(CURSOR_MODE_HARDWARE);
}

void CPythonApplication::MoveWebPage(const BYTE c_bType, const RECT& c_rcWebPage)
{
#if defined(ENABLE_CEF_BROWSER)
	if (CefWebBrowser::IsVisible(c_bType))
	{
		m_grpDevice.MoveWebBrowserRect(c_rcWebPage);
		CefWebBrowser::Move(c_bType, &c_rcWebPage);
	}
#else
	if (WebBrowser_IsVisible())
	{
		m_grpDevice.MoveWebBrowserRect(c_rcWebPage);
		WebBrowser_Move(&c_rcWebPage);
	}
#endif
}

void CPythonApplication::HideWebPage(const BYTE c_bType)
{
#if defined(ENABLE_CEF_BROWSER)
	if (CefWebBrowser::IsVisible(c_bType))
	{
		CefWebBrowser::Hide(c_bType);

		m_grpDevice.DisableWebBrowserMode();

		if (m_pySystem.IsSoftwareCursor())
			SetCursorMode(CURSOR_MODE_SOFTWARE);
		else
			SetCursorMode(CURSOR_MODE_HARDWARE);
	}
#else
	if (WebBrowser_IsVisible())
	{
		WebBrowser_Hide();

		m_grpDevice.DisableWebBrowserMode();

		if (m_pySystem.IsSoftwareCursor())
			SetCursorMode(CURSOR_MODE_SOFTWARE);
		else
			SetCursorMode(CURSOR_MODE_HARDWARE);
	}
#endif
}

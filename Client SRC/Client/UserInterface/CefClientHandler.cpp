#include "StdAfx.h"
#include "CefClientHandler.h"

#include <cef/cef_app.h>
#include <cef/cef_base.h>
#include <cef/cef_client.h>
#include <cef/cef_command_line.h>
#include <cef/cef_frame.h>
#include <cef/cef_web_plugin.h>

ClientHandler::ClientHandler() : m_BrowserHwnd(NULL) {}

bool ClientHandler::DoClose(CefRefPtr<CefBrowser> browser)
{
	return false;
}

void ClientHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
	if (!m_Browser.get())
	{
		// We need to keep the main child window, but not popup windows
		m_Browser = browser;
		m_BrowserHwnd = browser->GetHost()->GetWindowHandle();
	}
}

void ClientHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser)
{
	if (m_BrowserHwnd == browser->GetHost()->GetWindowHandle())
	{
		// Free the browser pointer so that the browser can be destroyed
		m_Browser = nullptr;
	}
}

void ClientHandler::OnBeforeContextMenu(
	CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	CefRefPtr<CefContextMenuParams> params,
	CefRefPtr<CefMenuModel> model)
{
	model->Clear();
}

bool ClientHandler::OnContextMenuCommand(
	CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	CefRefPtr<CefContextMenuParams> params,
	int command_id,
	EventFlags event_flags)
{
	return false;
}

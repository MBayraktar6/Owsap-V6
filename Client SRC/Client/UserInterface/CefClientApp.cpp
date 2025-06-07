#include "StdAfx.h"

#include "CefClientApp.h"
#include "CefClientHandler.h"

#include "cef/cef_browser.h"
#include "cef/cef_command_line.h"
#include "cef/views/cef_browser_view.h"
#include "cef/views/cef_window.h"
#include "cef/wrapper/cef_helpers.h"
#include "CefClientV8ExtensionHandler.h"

ClientApp::ClientApp()
{
}

void ClientApp::OnWebKitInitialized()
{
	const std::string app_code =
		"var app;"
		"if (!app)"
		"	app = {};"
		"(function() {"
		"	app.ChangeTextInJS = function(text) {"
		"		native function ChangeTextInJS();"
		"		return ChangeTextInJS(text);"
		"	};"
		"})();";

	CefRegisterExtension("v8/app", app_code, new ClientV8ExtensionHandler(this));
}

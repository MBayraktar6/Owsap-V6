#if !defined(__INC_CEF_CLIENT_APP_H__)
#define __INC_CEF_CLIENT_APP_H__

#include <cef/cef_app.h>

class ClientApp :
	public CefApp,
	public CefBrowserProcessHandler,
	public CefRenderProcessHandler
{
public:
	ClientApp();

	// CefApp methods:
	virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() OVERRIDE { return this; }

	// CefRenderProcessHandler methods:
	virtual void OnWebKitInitialized() OVERRIDE;

private:
	// Include the default reference counting implementation.
	IMPLEMENT_REFCOUNTING(ClientApp);
};
#endif // __INC_CEF_CLIENT_APP_H__

#ifndef __INC_CEF_CLIENT_V8_EXTENSION_HANDLER_H__
#define __INC_CEF_CLIENT_V8_EXTENSION_HANDLER_H__

#include <cef/cef_app.h>

struct ClientV8ExtensionHandler : public CefV8Handler
{
	ClientV8ExtensionHandler(CefRefPtr<CefApp> app);

	bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) override;

private:
	CefRefPtr<CefApp> app;

	IMPLEMENT_REFCOUNTING(ClientV8ExtensionHandler);
};

#endif // __INC_CEF_CLIENT_V8_EXTENSION_HANDLER_H__

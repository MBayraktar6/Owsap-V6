// Copyright (c) 2012 Marshall A. Greenblatt. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//    * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//    * Neither the name of Google Inc. nor the name Chromium Embedded
// Framework nor the names of its contributors may be used to endorse
// or promote products derived from this software without specific prior
// written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// ---------------------------------------------------------------------------
//
// The contents of this file must follow a specific format in order to
// support the CEF translator tool. See the translator.README.txt file in the
// tools directory for more information.
//

#ifndef CEF_INCLUDE_CEF_DISPLAY_HANDLER_H_
#define CEF_INCLUDE_CEF_DISPLAY_HANDLER_H_
#pragma once

#include "cef_base.h"
#include "cef_browser.h"
#include "cef_frame.h"

///
// Implement this interface to handle events related to browser display state.
// The methods of this class will be called on the UI thread.
///
/*--cef(source=client)--*/
class CefDisplayHandler : public virtual CefBaseRefCounted {
 public:
  ///
  // Called when a frame's address has changed.
  ///
  /*--cef()--*/
  virtual void OnAddressChange(CefRefPtr<CefBrowser> browser,
                               CefRefPtr<CefFrame> frame,
                               const CefString& url) {}

  ///
  // Called when the page title changes.
  ///
  /*--cef(optional_param=title)--*/
  virtual void OnTitleChange(CefRefPtr<CefBrowser> browser,
                             const CefString& title) {}

  ///
  // Called when the page icon changes.
  ///
  /*--cef(optional_param=icon_urls)--*/
  virtual void OnFaviconURLChange(CefRefPtr<CefBrowser> browser,
                                  const std::vector<CefString>& icon_urls) {}

  ///
  // Called when web content in the page has toggled fullscreen mode. If
  // |fullscreen| is true the content will automatically be sized to fill the
  // browser content area. If |fullscreen| is false the content will
  // automatically return to its original size and position. The client is
  // responsible for resizing the browser if desired.
  ///
  /*--cef()--*/
  virtual void OnFullscreenModeChange(CefRefPtr<CefBrowser> browser,
                                      bool fullscreen) {}

  ///
  // Called when the browser is about to display a tooltip. |text| contains the
  // text that will be displayed in the tooltip. To handle the display of the
  // tooltip yourself return true. Otherwise, you can optionally modify |text|
  // and then return false to allow the browser to display the tooltip.
  // When window rendering is disabled the application is responsible for
  // drawing tooltips and the return value is ignored.
  ///
  /*--cef(optional_param=text)--*/
  virtual bool OnTooltip(CefRefPtr<CefBrowser> browser, CefString& text) {
    return false;
  }

  ///
  // Called when the browser receives a status message. |value| contains the
  // text that will be displayed in the status message.
  ///
  /*--cef(optional_param=value)--*/
  virtual void OnStatusMessage(CefRefPtr<CefBrowser> browser,
                               const CefString& value) {}

  ///
  // Called to display a console message. Return true to stop the message from
  // being output to the console.
  ///
  /*--cef(optional_param=message,optional_param=source)--*/
  virtual bool OnConsoleMessage(CefRefPtr<CefBrowser> browser,
                                cef_log_severity_t level,
                                const CefString& message,
                                const CefString& source,
                                int line) {
    return false;
  }

  ///
  // Called when auto-resize is enabled via CefBrowserHost::SetAutoResizeEnabled
  // and the contents have auto-resized. |new_size| will be the desired size in
  // view coordinates. Return true if the resize was handled or false for
  // default handling.
  ///
  /*--cef()--*/
  virtual bool OnAutoResize(CefRefPtr<CefBrowser> browser,
                            const CefSize& new_size) {
    return false;
  }

  ///
  // Called when the overall page loading progress has changed. |progress|
  // ranges from 0.0 to 1.0.
  ///
  /*--cef()--*/
  virtual void OnLoadingProgressChange(CefRefPtr<CefBrowser> browser,
                                       double progress) {}

  ///
  // Called when the browser's cursor has changed. If |type| is CT_CUSTOM then
  // |custom_cursor_info| will be populated with the custom cursor information.
  // Return true if the cursor change was handled or false for default handling.
  ///
  /*--cef()--*/
  virtual bool OnCursorChange(CefRefPtr<CefBrowser> browser,
                              CefCursorHandle cursor,
                              cef_cursor_type_t type,
                              const CefCursorInfo& custom_cursor_info) {
    return false;
  }
};

#endif  // CEF_INCLUDE_CEF_DISPLAY_HANDLER_H_

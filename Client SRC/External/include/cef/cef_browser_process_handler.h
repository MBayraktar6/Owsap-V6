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

#ifndef CEF_INCLUDE_CEF_BROWSER_PROCESS_HANDLER_H_
#define CEF_INCLUDE_CEF_BROWSER_PROCESS_HANDLER_H_
#pragma once

#include "cef_base.h"
#include "cef_command_line.h"
#include "cef_print_handler.h"
#include "cef_values.h"

///
// Class used to implement browser process callbacks. The methods of this class
// will be called on the browser process main thread unless otherwise indicated.
///
/*--cef(source=client,no_debugct_check)--*/
class CefBrowserProcessHandler : public virtual CefBaseRefCounted {
 public:
  ///
  // Called on the browser process UI thread immediately after the CEF context
  // has been initialized.
  ///
  /*--cef()--*/
  virtual void OnContextInitialized() {}

  ///
  // Called before a child process is launched. Will be called on the browser
  // process UI thread when launching a render process and on the browser
  // process IO thread when launching a GPU or plugin process. Provides an
  // opportunity to modify the child process command line. Do not keep a
  // reference to |command_line| outside of this method.
  ///
  /*--cef()--*/
  virtual void OnBeforeChildProcessLaunch(
      CefRefPtr<CefCommandLine> command_line) {}

  ///
  // Called on the browser process IO thread after the main thread has been
  // created for a new render process. Provides an opportunity to specify extra
  // information that will be passed to
  // CefRenderProcessHandler::OnRenderThreadCreated() in the render process. Do
  // not keep a reference to |extra_info| outside of this method.
  ///
  /*--cef()--*/
  virtual void OnRenderProcessThreadCreated(
      CefRefPtr<CefListValue> extra_info) {}

  ///
  // Return the handler for printing on Linux. If a print handler is not
  // provided then printing will not be supported on the Linux platform.
  ///
  /*--cef()--*/
  virtual CefRefPtr<CefPrintHandler> GetPrintHandler() { return nullptr; }

  ///
  // Called from any thread when work has been scheduled for the browser process
  // main (UI) thread. This callback is used in combination with CefSettings.
  // external_message_pump and CefDoMessageLoopWork() in cases where the CEF
  // message loop must be integrated into an existing application message loop
  // (see additional comments and warnings on CefDoMessageLoopWork). This
  // callback should schedule a CefDoMessageLoopWork() call to happen on the
  // main (UI) thread. |delay_ms| is the requested delay in milliseconds. If
  // |delay_ms| is <= 0 then the call should happen reasonably soon. If
  // |delay_ms| is > 0 then the call should be scheduled to happen after the
  // specified delay and any currently pending scheduled call should be
  // cancelled.
  ///
  /*--cef()--*/
  virtual void OnScheduleMessagePumpWork(int64 delay_ms) {}
};

#endif  // CEF_INCLUDE_CEF_BROWSER_PROCESS_HANDLER_H_

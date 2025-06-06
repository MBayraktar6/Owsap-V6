// Copyright (c) 2020 Marshall A. Greenblatt. All rights reserved.
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
// This file was generated by the CEF translator tool and should not edited
// by hand. See the translator.README.txt file in the tools directory for
// more information.
//
// $hash=eada7e92085d96497f4e69f3e8a7e8aa6746b175$
//

#ifndef CEF_INCLUDE_CAPI_CEF_DISPLAY_HANDLER_CAPI_H_
#define CEF_INCLUDE_CAPI_CEF_DISPLAY_HANDLER_CAPI_H_
#pragma once

#include "capi/cef_base_capi.h"
#include "capi/cef_browser_capi.h"
#include "capi/cef_frame_capi.h"

#ifdef __cplusplus
extern "C" {
#endif

///
// Implement this structure to handle events related to browser display state.
// The functions of this structure will be called on the UI thread.
///
typedef struct _cef_display_handler_t {
  ///
  // Base structure.
  ///
  cef_base_ref_counted_t base;

  ///
  // Called when a frame's address has changed.
  ///
  void(CEF_CALLBACK* on_address_change)(struct _cef_display_handler_t* self,
                                        struct _cef_browser_t* browser,
                                        struct _cef_frame_t* frame,
                                        const cef_string_t* url);

  ///
  // Called when the page title changes.
  ///
  void(CEF_CALLBACK* on_title_change)(struct _cef_display_handler_t* self,
                                      struct _cef_browser_t* browser,
                                      const cef_string_t* title);

  ///
  // Called when the page icon changes.
  ///
  void(CEF_CALLBACK* on_favicon_urlchange)(struct _cef_display_handler_t* self,
                                           struct _cef_browser_t* browser,
                                           cef_string_list_t icon_urls);

  ///
  // Called when web content in the page has toggled fullscreen mode. If
  // |fullscreen| is true (1) the content will automatically be sized to fill
  // the browser content area. If |fullscreen| is false (0) the content will
  // automatically return to its original size and position. The client is
  // responsible for resizing the browser if desired.
  ///
  void(CEF_CALLBACK* on_fullscreen_mode_change)(
      struct _cef_display_handler_t* self,
      struct _cef_browser_t* browser,
      int fullscreen);

  ///
  // Called when the browser is about to display a tooltip. |text| contains the
  // text that will be displayed in the tooltip. To handle the display of the
  // tooltip yourself return true (1). Otherwise, you can optionally modify
  // |text| and then return false (0) to allow the browser to display the
  // tooltip. When window rendering is disabled the application is responsible
  // for drawing tooltips and the return value is ignored.
  ///
  int(CEF_CALLBACK* on_tooltip)(struct _cef_display_handler_t* self,
                                struct _cef_browser_t* browser,
                                cef_string_t* text);

  ///
  // Called when the browser receives a status message. |value| contains the
  // text that will be displayed in the status message.
  ///
  void(CEF_CALLBACK* on_status_message)(struct _cef_display_handler_t* self,
                                        struct _cef_browser_t* browser,
                                        const cef_string_t* value);

  ///
  // Called to display a console message. Return true (1) to stop the message
  // from being output to the console.
  ///
  int(CEF_CALLBACK* on_console_message)(struct _cef_display_handler_t* self,
                                        struct _cef_browser_t* browser,
                                        cef_log_severity_t level,
                                        const cef_string_t* message,
                                        const cef_string_t* source,
                                        int line);

  ///
  // Called when auto-resize is enabled via
  // cef_browser_host_t::SetAutoResizeEnabled and the contents have auto-
  // resized. |new_size| will be the desired size in view coordinates. Return
  // true (1) if the resize was handled or false (0) for default handling.
  ///
  int(CEF_CALLBACK* on_auto_resize)(struct _cef_display_handler_t* self,
                                    struct _cef_browser_t* browser,
                                    const cef_size_t* new_size);

  ///
  // Called when the overall page loading progress has changed. |progress|
  // ranges from 0.0 to 1.0.
  ///
  void(CEF_CALLBACK* on_loading_progress_change)(
      struct _cef_display_handler_t* self,
      struct _cef_browser_t* browser,
      double progress);

  ///
  // Called when the browser's cursor has changed. If |type| is CT_CUSTOM then
  // |custom_cursor_info| will be populated with the custom cursor information.
  // Return true (1) if the cursor change was handled or false (0) for default
  // handling.
  ///
  int(CEF_CALLBACK* on_cursor_change)(
      struct _cef_display_handler_t* self,
      struct _cef_browser_t* browser,
      cef_cursor_handle_t cursor,
      cef_cursor_type_t type,
      const struct _cef_cursor_info_t* custom_cursor_info);
} cef_display_handler_t;

#ifdef __cplusplus
}
#endif

#endif  // CEF_INCLUDE_CAPI_CEF_DISPLAY_HANDLER_CAPI_H_

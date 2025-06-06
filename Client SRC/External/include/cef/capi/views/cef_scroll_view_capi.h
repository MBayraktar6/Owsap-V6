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
// $hash=3b9789f2149c3e6d6a2e671e5427c19654442ede$
//

#ifndef CEF_INCLUDE_CAPI_VIEWS_CEF_SCROLL_VIEW_CAPI_H_
#define CEF_INCLUDE_CAPI_VIEWS_CEF_SCROLL_VIEW_CAPI_H_
#pragma once

#include "capi/views/cef_view_capi.h"

#ifdef __cplusplus
extern "C" {
#endif

///
// A ScrollView will show horizontal and/or vertical scrollbars when necessary
// based on the size of the attached content view. Methods must be called on the
// browser process UI thread unless otherwise indicated.
///
typedef struct _cef_scroll_view_t {
  ///
  // Base structure.
  ///
  cef_view_t base;

  ///
  // Set the content View. The content View must have a specified size (e.g. via
  // cef_view_t::SetBounds or cef_view_delegate_t::GetPreferredSize).
  ///
  void(CEF_CALLBACK* set_content_view)(struct _cef_scroll_view_t* self,
                                       struct _cef_view_t* view);

  ///
  // Returns the content View.
  ///
  struct _cef_view_t*(CEF_CALLBACK* get_content_view)(
      struct _cef_scroll_view_t* self);

  ///
  // Returns the visible region of the content View.
  ///
  cef_rect_t(CEF_CALLBACK* get_visible_content_rect)(
      struct _cef_scroll_view_t* self);

  ///
  // Returns true (1) if the horizontal scrollbar is currently showing.
  ///
  int(CEF_CALLBACK* has_horizontal_scrollbar)(struct _cef_scroll_view_t* self);

  ///
  // Returns the height of the horizontal scrollbar.
  ///
  int(CEF_CALLBACK* get_horizontal_scrollbar_height)(
      struct _cef_scroll_view_t* self);

  ///
  // Returns true (1) if the vertical scrollbar is currently showing.
  ///
  int(CEF_CALLBACK* has_vertical_scrollbar)(struct _cef_scroll_view_t* self);

  ///
  // Returns the width of the vertical scrollbar.
  ///
  int(CEF_CALLBACK* get_vertical_scrollbar_width)(
      struct _cef_scroll_view_t* self);
} cef_scroll_view_t;

///
// Create a new ScrollView.
///
CEF_EXPORT cef_scroll_view_t* cef_scroll_view_create(
    struct _cef_view_delegate_t* delegate);

#ifdef __cplusplus
}
#endif

#endif  // CEF_INCLUDE_CAPI_VIEWS_CEF_SCROLL_VIEW_CAPI_H_

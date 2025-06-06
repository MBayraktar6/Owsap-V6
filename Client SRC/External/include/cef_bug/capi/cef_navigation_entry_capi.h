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
// $hash=c6252024911652a4881d753aeeeb2615e6be3904$
//

#ifndef CEF_INCLUDE_CAPI_CEF_NAVIGATION_ENTRY_CAPI_H_
#define CEF_INCLUDE_CAPI_CEF_NAVIGATION_ENTRY_CAPI_H_
#pragma once

#include "capi/cef_base_capi.h"
#include "capi/cef_ssl_status_capi.h"

#ifdef __cplusplus
extern "C" {
#endif

///
// Structure used to represent an entry in navigation history.
///
typedef struct _cef_navigation_entry_t {
  ///
  // Base structure.
  ///
  cef_base_ref_counted_t base;

  ///
  // Returns true (1) if this object is valid. Do not call any other functions
  // if this function returns false (0).
  ///
  int(CEF_CALLBACK* is_valid)(struct _cef_navigation_entry_t* self);

  ///
  // Returns the actual URL of the page. For some pages this may be data: URL or
  // similar. Use get_display_url() to return a display-friendly version.
  ///
  // The resulting string must be freed by calling cef_string_userfree_free().
  cef_string_userfree_t(CEF_CALLBACK* get_url)(
      struct _cef_navigation_entry_t* self);

  ///
  // Returns a display-friendly version of the URL.
  ///
  // The resulting string must be freed by calling cef_string_userfree_free().
  cef_string_userfree_t(CEF_CALLBACK* get_display_url)(
      struct _cef_navigation_entry_t* self);

  ///
  // Returns the original URL that was entered by the user before any redirects.
  ///
  // The resulting string must be freed by calling cef_string_userfree_free().
  cef_string_userfree_t(CEF_CALLBACK* get_original_url)(
      struct _cef_navigation_entry_t* self);

  ///
  // Returns the title set by the page. This value may be NULL.
  ///
  // The resulting string must be freed by calling cef_string_userfree_free().
  cef_string_userfree_t(CEF_CALLBACK* get_title)(
      struct _cef_navigation_entry_t* self);

  ///
  // Returns the transition type which indicates what the user did to move to
  // this page from the previous page.
  ///
  cef_transition_type_t(CEF_CALLBACK* get_transition_type)(
      struct _cef_navigation_entry_t* self);

  ///
  // Returns true (1) if this navigation includes post data.
  ///
  int(CEF_CALLBACK* has_post_data)(struct _cef_navigation_entry_t* self);

  ///
  // Returns the time for the last known successful navigation completion. A
  // navigation may be completed more than once if the page is reloaded. May be
  // 0 if the navigation has not yet completed.
  ///
  cef_time_t(CEF_CALLBACK* get_completion_time)(
      struct _cef_navigation_entry_t* self);

  ///
  // Returns the HTTP status code for the last known successful navigation
  // response. May be 0 if the response has not yet been received or if the
  // navigation has not yet completed.
  ///
  int(CEF_CALLBACK* get_http_status_code)(struct _cef_navigation_entry_t* self);

  ///
  // Returns the SSL information for this navigation entry.
  ///
  struct _cef_sslstatus_t*(CEF_CALLBACK* get_sslstatus)(
      struct _cef_navigation_entry_t* self);
} cef_navigation_entry_t;

#ifdef __cplusplus
}
#endif

#endif  // CEF_INCLUDE_CAPI_CEF_NAVIGATION_ENTRY_CAPI_H_

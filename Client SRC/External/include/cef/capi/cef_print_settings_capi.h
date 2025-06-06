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
// $hash=8f7d7993691e07f4a8a42d63522c751cfba3c168$
//

#ifndef CEF_INCLUDE_CAPI_CEF_PRINT_SETTINGS_CAPI_H_
#define CEF_INCLUDE_CAPI_CEF_PRINT_SETTINGS_CAPI_H_
#pragma once

#include "capi/cef_base_capi.h"

#ifdef __cplusplus
extern "C" {
#endif

///
// Structure representing print settings.
///
typedef struct _cef_print_settings_t {
  ///
  // Base structure.
  ///
  cef_base_ref_counted_t base;

  ///
  // Returns true (1) if this object is valid. Do not call any other functions
  // if this function returns false (0).
  ///
  int(CEF_CALLBACK* is_valid)(struct _cef_print_settings_t* self);

  ///
  // Returns true (1) if the values of this object are read-only. Some APIs may
  // expose read-only objects.
  ///
  int(CEF_CALLBACK* is_read_only)(struct _cef_print_settings_t* self);

  ///
  // Set the page orientation.
  ///
  void(CEF_CALLBACK* set_orientation)(struct _cef_print_settings_t* self,
                                      int landscape);

  ///
  // Returns true (1) if the orientation is landscape.
  ///
  int(CEF_CALLBACK* is_landscape)(struct _cef_print_settings_t* self);

  ///
  // Set the printer printable area in device units. Some platforms already
  // provide flipped area. Set |landscape_needs_flip| to false (0) on those
  // platforms to avoid double flipping.
  ///
  void(CEF_CALLBACK* set_printer_printable_area)(
      struct _cef_print_settings_t* self,
      const cef_size_t* physical_size_device_units,
      const cef_rect_t* printable_area_device_units,
      int landscape_needs_flip);

  ///
  // Set the device name.
  ///
  void(CEF_CALLBACK* set_device_name)(struct _cef_print_settings_t* self,
                                      const cef_string_t* name);

  ///
  // Get the device name.
  ///
  // The resulting string must be freed by calling cef_string_userfree_free().
  cef_string_userfree_t(CEF_CALLBACK* get_device_name)(
      struct _cef_print_settings_t* self);

  ///
  // Set the DPI (dots per inch).
  ///
  void(CEF_CALLBACK* set_dpi)(struct _cef_print_settings_t* self, int dpi);

  ///
  // Get the DPI (dots per inch).
  ///
  int(CEF_CALLBACK* get_dpi)(struct _cef_print_settings_t* self);

  ///
  // Set the page ranges.
  ///
  void(CEF_CALLBACK* set_page_ranges)(struct _cef_print_settings_t* self,
                                      size_t rangesCount,
                                      cef_range_t const* ranges);

  ///
  // Returns the number of page ranges that currently exist.
  ///
  size_t(CEF_CALLBACK* get_page_ranges_count)(
      struct _cef_print_settings_t* self);

  ///
  // Retrieve the page ranges.
  ///
  void(CEF_CALLBACK* get_page_ranges)(struct _cef_print_settings_t* self,
                                      size_t* rangesCount,
                                      cef_range_t* ranges);

  ///
  // Set whether only the selection will be printed.
  ///
  void(CEF_CALLBACK* set_selection_only)(struct _cef_print_settings_t* self,
                                         int selection_only);

  ///
  // Returns true (1) if only the selection will be printed.
  ///
  int(CEF_CALLBACK* is_selection_only)(struct _cef_print_settings_t* self);

  ///
  // Set whether pages will be collated.
  ///
  void(CEF_CALLBACK* set_collate)(struct _cef_print_settings_t* self,
                                  int collate);

  ///
  // Returns true (1) if pages will be collated.
  ///
  int(CEF_CALLBACK* will_collate)(struct _cef_print_settings_t* self);

  ///
  // Set the color model.
  ///
  void(CEF_CALLBACK* set_color_model)(struct _cef_print_settings_t* self,
                                      cef_color_model_t model);

  ///
  // Get the color model.
  ///
  cef_color_model_t(CEF_CALLBACK* get_color_model)(
      struct _cef_print_settings_t* self);

  ///
  // Set the number of copies.
  ///
  void(CEF_CALLBACK* set_copies)(struct _cef_print_settings_t* self,
                                 int copies);

  ///
  // Get the number of copies.
  ///
  int(CEF_CALLBACK* get_copies)(struct _cef_print_settings_t* self);

  ///
  // Set the duplex mode.
  ///
  void(CEF_CALLBACK* set_duplex_mode)(struct _cef_print_settings_t* self,
                                      cef_duplex_mode_t mode);

  ///
  // Get the duplex mode.
  ///
  cef_duplex_mode_t(CEF_CALLBACK* get_duplex_mode)(
      struct _cef_print_settings_t* self);
} cef_print_settings_t;

///
// Create a new cef_print_settings_t object.
///
CEF_EXPORT cef_print_settings_t* cef_print_settings_create();

#ifdef __cplusplus
}
#endif

#endif  // CEF_INCLUDE_CAPI_CEF_PRINT_SETTINGS_CAPI_H_

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
// $hash=cce24dba079162b10f359769eea176c4009b5ce5$
//

#ifndef CEF_INCLUDE_CAPI_CEF_MENU_MODEL_CAPI_H_
#define CEF_INCLUDE_CAPI_CEF_MENU_MODEL_CAPI_H_
#pragma once

#include "capi/cef_base_capi.h"
#include "capi/cef_menu_model_delegate_capi.h"

#ifdef __cplusplus
extern "C" {
#endif

///
// Supports creation and modification of menus. See cef_menu_id_t for the
// command ids that have default implementations. All user-defined command ids
// should be between MENU_ID_USER_FIRST and MENU_ID_USER_LAST. The functions of
// this structure can only be accessed on the browser process the UI thread.
///
typedef struct _cef_menu_model_t {
  ///
  // Base structure.
  ///
  cef_base_ref_counted_t base;

  ///
  // Returns true (1) if this menu is a submenu.
  ///
  int(CEF_CALLBACK* is_sub_menu)(struct _cef_menu_model_t* self);

  ///
  // Clears the menu. Returns true (1) on success.
  ///
  int(CEF_CALLBACK* clear)(struct _cef_menu_model_t* self);

  ///
  // Returns the number of items in this menu.
  ///
  int(CEF_CALLBACK* get_count)(struct _cef_menu_model_t* self);

  ///
  // Add a separator to the menu. Returns true (1) on success.
  ///
  int(CEF_CALLBACK* add_separator)(struct _cef_menu_model_t* self);

  ///
  // Add an item to the menu. Returns true (1) on success.
  ///
  int(CEF_CALLBACK* add_item)(struct _cef_menu_model_t* self,
                              int command_id,
                              const cef_string_t* label);

  ///
  // Add a check item to the menu. Returns true (1) on success.
  ///
  int(CEF_CALLBACK* add_check_item)(struct _cef_menu_model_t* self,
                                    int command_id,
                                    const cef_string_t* label);

  ///
  // Add a radio item to the menu. Only a single item with the specified
  // |group_id| can be checked at a time. Returns true (1) on success.
  ///
  int(CEF_CALLBACK* add_radio_item)(struct _cef_menu_model_t* self,
                                    int command_id,
                                    const cef_string_t* label,
                                    int group_id);

  ///
  // Add a sub-menu to the menu. The new sub-menu is returned.
  ///
  struct _cef_menu_model_t*(CEF_CALLBACK* add_sub_menu)(
      struct _cef_menu_model_t* self,
      int command_id,
      const cef_string_t* label);

  ///
  // Insert a separator in the menu at the specified |index|. Returns true (1)
  // on success.
  ///
  int(CEF_CALLBACK* insert_separator_at)(struct _cef_menu_model_t* self,
                                         int index);

  ///
  // Insert an item in the menu at the specified |index|. Returns true (1) on
  // success.
  ///
  int(CEF_CALLBACK* insert_item_at)(struct _cef_menu_model_t* self,
                                    int index,
                                    int command_id,
                                    const cef_string_t* label);

  ///
  // Insert a check item in the menu at the specified |index|. Returns true (1)
  // on success.
  ///
  int(CEF_CALLBACK* insert_check_item_at)(struct _cef_menu_model_t* self,
                                          int index,
                                          int command_id,
                                          const cef_string_t* label);

  ///
  // Insert a radio item in the menu at the specified |index|. Only a single
  // item with the specified |group_id| can be checked at a time. Returns true
  // (1) on success.
  ///
  int(CEF_CALLBACK* insert_radio_item_at)(struct _cef_menu_model_t* self,
                                          int index,
                                          int command_id,
                                          const cef_string_t* label,
                                          int group_id);

  ///
  // Insert a sub-menu in the menu at the specified |index|. The new sub-menu is
  // returned.
  ///
  struct _cef_menu_model_t*(CEF_CALLBACK* insert_sub_menu_at)(
      struct _cef_menu_model_t* self,
      int index,
      int command_id,
      const cef_string_t* label);

  ///
  // Removes the item with the specified |command_id|. Returns true (1) on
  // success.
  ///
  int(CEF_CALLBACK* remove)(struct _cef_menu_model_t* self, int command_id);

  ///
  // Removes the item at the specified |index|. Returns true (1) on success.
  ///
  int(CEF_CALLBACK* remove_at)(struct _cef_menu_model_t* self, int index);

  ///
  // Returns the index associated with the specified |command_id| or -1 if not
  // found due to the command id not existing in the menu.
  ///
  int(CEF_CALLBACK* get_index_of)(struct _cef_menu_model_t* self,
                                  int command_id);

  ///
  // Returns the command id at the specified |index| or -1 if not found due to
  // invalid range or the index being a separator.
  ///
  int(CEF_CALLBACK* get_command_id_at)(struct _cef_menu_model_t* self,
                                       int index);

  ///
  // Sets the command id at the specified |index|. Returns true (1) on success.
  ///
  int(CEF_CALLBACK* set_command_id_at)(struct _cef_menu_model_t* self,
                                       int index,
                                       int command_id);

  ///
  // Returns the label for the specified |command_id| or NULL if not found.
  ///
  // The resulting string must be freed by calling cef_string_userfree_free().
  cef_string_userfree_t(CEF_CALLBACK* get_label)(struct _cef_menu_model_t* self,
                                                 int command_id);

  ///
  // Returns the label at the specified |index| or NULL if not found due to
  // invalid range or the index being a separator.
  ///
  // The resulting string must be freed by calling cef_string_userfree_free().
  cef_string_userfree_t(
      CEF_CALLBACK* get_label_at)(struct _cef_menu_model_t* self, int index);

  ///
  // Sets the label for the specified |command_id|. Returns true (1) on success.
  ///
  int(CEF_CALLBACK* set_label)(struct _cef_menu_model_t* self,
                               int command_id,
                               const cef_string_t* label);

  ///
  // Set the label at the specified |index|. Returns true (1) on success.
  ///
  int(CEF_CALLBACK* set_label_at)(struct _cef_menu_model_t* self,
                                  int index,
                                  const cef_string_t* label);

  ///
  // Returns the item type for the specified |command_id|.
  ///
  cef_menu_item_type_t(CEF_CALLBACK* get_type)(struct _cef_menu_model_t* self,
                                               int command_id);

  ///
  // Returns the item type at the specified |index|.
  ///
  cef_menu_item_type_t(
      CEF_CALLBACK* get_type_at)(struct _cef_menu_model_t* self, int index);

  ///
  // Returns the group id for the specified |command_id| or -1 if invalid.
  ///
  int(CEF_CALLBACK* get_group_id)(struct _cef_menu_model_t* self,
                                  int command_id);

  ///
  // Returns the group id at the specified |index| or -1 if invalid.
  ///
  int(CEF_CALLBACK* get_group_id_at)(struct _cef_menu_model_t* self, int index);

  ///
  // Sets the group id for the specified |command_id|. Returns true (1) on
  // success.
  ///
  int(CEF_CALLBACK* set_group_id)(struct _cef_menu_model_t* self,
                                  int command_id,
                                  int group_id);

  ///
  // Sets the group id at the specified |index|. Returns true (1) on success.
  ///
  int(CEF_CALLBACK* set_group_id_at)(struct _cef_menu_model_t* self,
                                     int index,
                                     int group_id);

  ///
  // Returns the submenu for the specified |command_id| or NULL if invalid.
  ///
  struct _cef_menu_model_t*(CEF_CALLBACK* get_sub_menu)(
      struct _cef_menu_model_t* self,
      int command_id);

  ///
  // Returns the submenu at the specified |index| or NULL if invalid.
  ///
  struct _cef_menu_model_t*(
      CEF_CALLBACK* get_sub_menu_at)(struct _cef_menu_model_t* self, int index);

  ///
  // Returns true (1) if the specified |command_id| is visible.
  ///
  int(CEF_CALLBACK* is_visible)(struct _cef_menu_model_t* self, int command_id);

  ///
  // Returns true (1) if the specified |index| is visible.
  ///
  int(CEF_CALLBACK* is_visible_at)(struct _cef_menu_model_t* self, int index);

  ///
  // Change the visibility of the specified |command_id|. Returns true (1) on
  // success.
  ///
  int(CEF_CALLBACK* set_visible)(struct _cef_menu_model_t* self,
                                 int command_id,
                                 int visible);

  ///
  // Change the visibility at the specified |index|. Returns true (1) on
  // success.
  ///
  int(CEF_CALLBACK* set_visible_at)(struct _cef_menu_model_t* self,
                                    int index,
                                    int visible);

  ///
  // Returns true (1) if the specified |command_id| is enabled.
  ///
  int(CEF_CALLBACK* is_enabled)(struct _cef_menu_model_t* self, int command_id);

  ///
  // Returns true (1) if the specified |index| is enabled.
  ///
  int(CEF_CALLBACK* is_enabled_at)(struct _cef_menu_model_t* self, int index);

  ///
  // Change the enabled status of the specified |command_id|. Returns true (1)
  // on success.
  ///
  int(CEF_CALLBACK* set_enabled)(struct _cef_menu_model_t* self,
                                 int command_id,
                                 int enabled);

  ///
  // Change the enabled status at the specified |index|. Returns true (1) on
  // success.
  ///
  int(CEF_CALLBACK* set_enabled_at)(struct _cef_menu_model_t* self,
                                    int index,
                                    int enabled);

  ///
  // Returns true (1) if the specified |command_id| is checked. Only applies to
  // check and radio items.
  ///
  int(CEF_CALLBACK* is_checked)(struct _cef_menu_model_t* self, int command_id);

  ///
  // Returns true (1) if the specified |index| is checked. Only applies to check
  // and radio items.
  ///
  int(CEF_CALLBACK* is_checked_at)(struct _cef_menu_model_t* self, int index);

  ///
  // Check the specified |command_id|. Only applies to check and radio items.
  // Returns true (1) on success.
  ///
  int(CEF_CALLBACK* set_checked)(struct _cef_menu_model_t* self,
                                 int command_id,
                                 int checked);

  ///
  // Check the specified |index|. Only applies to check and radio items. Returns
  // true (1) on success.
  ///
  int(CEF_CALLBACK* set_checked_at)(struct _cef_menu_model_t* self,
                                    int index,
                                    int checked);

  ///
  // Returns true (1) if the specified |command_id| has a keyboard accelerator
  // assigned.
  ///
  int(CEF_CALLBACK* has_accelerator)(struct _cef_menu_model_t* self,
                                     int command_id);

  ///
  // Returns true (1) if the specified |index| has a keyboard accelerator
  // assigned.
  ///
  int(CEF_CALLBACK* has_accelerator_at)(struct _cef_menu_model_t* self,
                                        int index);

  ///
  // Set the keyboard accelerator for the specified |command_id|. |key_code| can
  // be any virtual key or character value. Returns true (1) on success.
  ///
  int(CEF_CALLBACK* set_accelerator)(struct _cef_menu_model_t* self,
                                     int command_id,
                                     int key_code,
                                     int shift_pressed,
                                     int ctrl_pressed,
                                     int alt_pressed);

  ///
  // Set the keyboard accelerator at the specified |index|. |key_code| can be
  // any virtual key or character value. Returns true (1) on success.
  ///
  int(CEF_CALLBACK* set_accelerator_at)(struct _cef_menu_model_t* self,
                                        int index,
                                        int key_code,
                                        int shift_pressed,
                                        int ctrl_pressed,
                                        int alt_pressed);

  ///
  // Remove the keyboard accelerator for the specified |command_id|. Returns
  // true (1) on success.
  ///
  int(CEF_CALLBACK* remove_accelerator)(struct _cef_menu_model_t* self,
                                        int command_id);

  ///
  // Remove the keyboard accelerator at the specified |index|. Returns true (1)
  // on success.
  ///
  int(CEF_CALLBACK* remove_accelerator_at)(struct _cef_menu_model_t* self,
                                           int index);

  ///
  // Retrieves the keyboard accelerator for the specified |command_id|. Returns
  // true (1) on success.
  ///
  int(CEF_CALLBACK* get_accelerator)(struct _cef_menu_model_t* self,
                                     int command_id,
                                     int* key_code,
                                     int* shift_pressed,
                                     int* ctrl_pressed,
                                     int* alt_pressed);

  ///
  // Retrieves the keyboard accelerator for the specified |index|. Returns true
  // (1) on success.
  ///
  int(CEF_CALLBACK* get_accelerator_at)(struct _cef_menu_model_t* self,
                                        int index,
                                        int* key_code,
                                        int* shift_pressed,
                                        int* ctrl_pressed,
                                        int* alt_pressed);

  ///
  // Set the explicit color for |command_id| and |color_type| to |color|.
  // Specify a |color| value of 0 to remove the explicit color. If no explicit
  // color or default color is set for |color_type| then the system color will
  // be used. Returns true (1) on success.
  ///
  int(CEF_CALLBACK* set_color)(struct _cef_menu_model_t* self,
                               int command_id,
                               cef_menu_color_type_t color_type,
                               cef_color_t color);

  ///
  // Set the explicit color for |command_id| and |index| to |color|. Specify a
  // |color| value of 0 to remove the explicit color. Specify an |index| value
  // of -1 to set the default color for items that do not have an explicit color
  // set. If no explicit color or default color is set for |color_type| then the
  // system color will be used. Returns true (1) on success.
  ///
  int(CEF_CALLBACK* set_color_at)(struct _cef_menu_model_t* self,
                                  int index,
                                  cef_menu_color_type_t color_type,
                                  cef_color_t color);

  ///
  // Returns in |color| the color that was explicitly set for |command_id| and
  // |color_type|. If a color was not set then 0 will be returned in |color|.
  // Returns true (1) on success.
  ///
  int(CEF_CALLBACK* get_color)(struct _cef_menu_model_t* self,
                               int command_id,
                               cef_menu_color_type_t color_type,
                               cef_color_t* color);

  ///
  // Returns in |color| the color that was explicitly set for |command_id| and
  // |color_type|. Specify an |index| value of -1 to return the default color in
  // |color|. If a color was not set then 0 will be returned in |color|. Returns
  // true (1) on success.
  ///
  int(CEF_CALLBACK* get_color_at)(struct _cef_menu_model_t* self,
                                  int index,
                                  cef_menu_color_type_t color_type,
                                  cef_color_t* color);

  ///
  // Sets the font list for the specified |command_id|. If |font_list| is NULL
  // the system font will be used. Returns true (1) on success. The format is
  // "<FONT_FAMILY_LIST>,[STYLES] <SIZE>", where: - FONT_FAMILY_LIST is a comma-
  // separated list of font family names, - STYLES is an optional space-
  // separated list of style names (case-sensitive
  //   "Bold" and "Italic" are supported), and
  // - SIZE is an integer font size in pixels with the suffix "px".
  //
  // Here are examples of valid font description strings: - "Arial, Helvetica,
  // Bold Italic 14px" - "Arial, 14px"
  ///
  int(CEF_CALLBACK* set_font_list)(struct _cef_menu_model_t* self,
                                   int command_id,
                                   const cef_string_t* font_list);

  ///
  // Sets the font list for the specified |index|. Specify an |index| value of
  // -1 to set the default font. If |font_list| is NULL the system font will be
  // used. Returns true (1) on success. The format is
  // "<FONT_FAMILY_LIST>,[STYLES] <SIZE>", where: - FONT_FAMILY_LIST is a comma-
  // separated list of font family names, - STYLES is an optional space-
  // separated list of style names (case-sensitive
  //   "Bold" and "Italic" are supported), and
  // - SIZE is an integer font size in pixels with the suffix "px".
  //
  // Here are examples of valid font description strings: - "Arial, Helvetica,
  // Bold Italic 14px" - "Arial, 14px"
  ///
  int(CEF_CALLBACK* set_font_list_at)(struct _cef_menu_model_t* self,
                                      int index,
                                      const cef_string_t* font_list);
} cef_menu_model_t;

///
// Create a new MenuModel with the specified |delegate|.
///
CEF_EXPORT cef_menu_model_t* cef_menu_model_create(
    struct _cef_menu_model_delegate_t* delegate);

#ifdef __cplusplus
}
#endif

#endif  // CEF_INCLUDE_CAPI_CEF_MENU_MODEL_CAPI_H_

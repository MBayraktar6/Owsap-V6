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
// $hash=7ce0953f069204a4dd2037c4a05ac9454c5e66a6$
//

#ifndef CEF_INCLUDE_CAPI_CEF_REQUEST_CONTEXT_CAPI_H_
#define CEF_INCLUDE_CAPI_CEF_REQUEST_CONTEXT_CAPI_H_
#pragma once

#include "capi/cef_callback_capi.h"
#include "capi/cef_cookie_capi.h"
#include "capi/cef_extension_capi.h"
#include "capi/cef_extension_handler_capi.h"
#include "capi/cef_media_router_capi.h"
#include "capi/cef_values_capi.h"

#ifdef __cplusplus
extern "C" {
#endif

struct _cef_request_context_handler_t;
struct _cef_scheme_handler_factory_t;

///
// Callback structure for cef_request_context_t::ResolveHost.
///
typedef struct _cef_resolve_callback_t {
  ///
  // Base structure.
  ///
  cef_base_ref_counted_t base;

  ///
  // Called on the UI thread after the ResolveHost request has completed.
  // |result| will be the result code. |resolved_ips| will be the list of
  // resolved IP addresses or NULL if the resolution failed.
  ///
  void(CEF_CALLBACK* on_resolve_completed)(struct _cef_resolve_callback_t* self,
                                           cef_errorcode_t result,
                                           cef_string_list_t resolved_ips);
} cef_resolve_callback_t;

///
// A request context provides request handling for a set of related browser or
// URL request objects. A request context can be specified when creating a new
// browser via the cef_browser_host_t static factory functions or when creating
// a new URL request via the cef_urlrequest_t static factory functions. Browser
// objects with different request contexts will never be hosted in the same
// render process. Browser objects with the same request context may or may not
// be hosted in the same render process depending on the process model. Browser
// objects created indirectly via the JavaScript window.open function or
// targeted links will share the same render process and the same request
// context as the source browser. When running in single-process mode there is
// only a single render process (the main process) and so all browsers created
// in single-process mode will share the same request context. This will be the
// first request context passed into a cef_browser_host_t static factory
// function and all other request context objects will be ignored.
///
typedef struct _cef_request_context_t {
  ///
  // Base structure.
  ///
  cef_base_ref_counted_t base;

  ///
  // Returns true (1) if this object is pointing to the same context as |that|
  // object.
  ///
  int(CEF_CALLBACK* is_same)(struct _cef_request_context_t* self,
                             struct _cef_request_context_t* other);

  ///
  // Returns true (1) if this object is sharing the same storage as |that|
  // object.
  ///
  int(CEF_CALLBACK* is_sharing_with)(struct _cef_request_context_t* self,
                                     struct _cef_request_context_t* other);

  ///
  // Returns true (1) if this object is the global context. The global context
  // is used by default when creating a browser or URL request with a NULL
  // context argument.
  ///
  int(CEF_CALLBACK* is_global)(struct _cef_request_context_t* self);

  ///
  // Returns the handler for this context if any.
  ///
  struct _cef_request_context_handler_t*(CEF_CALLBACK* get_handler)(
      struct _cef_request_context_t* self);

  ///
  // Returns the cache path for this object. If NULL an "incognito mode" in-
  // memory cache is being used.
  ///
  // The resulting string must be freed by calling cef_string_userfree_free().
  cef_string_userfree_t(CEF_CALLBACK* get_cache_path)(
      struct _cef_request_context_t* self);

  ///
  // Returns the cookie manager for this object. If |callback| is non-NULL it
  // will be executed asnychronously on the IO thread after the manager's
  // storage has been initialized.
  ///
  struct _cef_cookie_manager_t*(CEF_CALLBACK* get_cookie_manager)(
      struct _cef_request_context_t* self,
      struct _cef_completion_callback_t* callback);

  ///
  // Register a scheme handler factory for the specified |scheme_name| and
  // optional |domain_name|. An NULL |domain_name| value for a standard scheme
  // will cause the factory to match all domain names. The |domain_name| value
  // will be ignored for non-standard schemes. If |scheme_name| is a built-in
  // scheme and no handler is returned by |factory| then the built-in scheme
  // handler factory will be called. If |scheme_name| is a custom scheme then
  // you must also implement the cef_app_t::on_register_custom_schemes()
  // function in all processes. This function may be called multiple times to
  // change or remove the factory that matches the specified |scheme_name| and
  // optional |domain_name|. Returns false (0) if an error occurs. This function
  // may be called on any thread in the browser process.
  ///
  int(CEF_CALLBACK* register_scheme_handler_factory)(
      struct _cef_request_context_t* self,
      const cef_string_t* scheme_name,
      const cef_string_t* domain_name,
      struct _cef_scheme_handler_factory_t* factory);

  ///
  // Clear all registered scheme handler factories. Returns false (0) on error.
  // This function may be called on any thread in the browser process.
  ///
  int(CEF_CALLBACK* clear_scheme_handler_factories)(
      struct _cef_request_context_t* self);

  ///
  // Tells all renderer processes associated with this context to throw away
  // their plugin list cache. If |reload_pages| is true (1) they will also
  // reload all pages with plugins.
  // cef_request_context_handler_t::OnBeforePluginLoad may be called to rebuild
  // the plugin list cache.
  ///
  void(CEF_CALLBACK* purge_plugin_list_cache)(
      struct _cef_request_context_t* self,
      int reload_pages);

  ///
  // Returns true (1) if a preference with the specified |name| exists. This
  // function must be called on the browser process UI thread.
  ///
  int(CEF_CALLBACK* has_preference)(struct _cef_request_context_t* self,
                                    const cef_string_t* name);

  ///
  // Returns the value for the preference with the specified |name|. Returns
  // NULL if the preference does not exist. The returned object contains a copy
  // of the underlying preference value and modifications to the returned object
  // will not modify the underlying preference value. This function must be
  // called on the browser process UI thread.
  ///
  struct _cef_value_t*(CEF_CALLBACK* get_preference)(
      struct _cef_request_context_t* self,
      const cef_string_t* name);

  ///
  // Returns all preferences as a dictionary. If |include_defaults| is true (1)
  // then preferences currently at their default value will be included. The
  // returned object contains a copy of the underlying preference values and
  // modifications to the returned object will not modify the underlying
  // preference values. This function must be called on the browser process UI
  // thread.
  ///
  struct _cef_dictionary_value_t*(CEF_CALLBACK* get_all_preferences)(
      struct _cef_request_context_t* self,
      int include_defaults);

  ///
  // Returns true (1) if the preference with the specified |name| can be
  // modified using SetPreference. As one example preferences set via the
  // command-line usually cannot be modified. This function must be called on
  // the browser process UI thread.
  ///
  int(CEF_CALLBACK* can_set_preference)(struct _cef_request_context_t* self,
                                        const cef_string_t* name);

  ///
  // Set the |value| associated with preference |name|. Returns true (1) if the
  // value is set successfully and false (0) otherwise. If |value| is NULL the
  // preference will be restored to its default value. If setting the preference
  // fails then |error| will be populated with a detailed description of the
  // problem. This function must be called on the browser process UI thread.
  ///
  int(CEF_CALLBACK* set_preference)(struct _cef_request_context_t* self,
                                    const cef_string_t* name,
                                    struct _cef_value_t* value,
                                    cef_string_t* error);

  ///
  // Clears all certificate exceptions that were added as part of handling
  // cef_request_handler_t::on_certificate_error(). If you call this it is
  // recommended that you also call close_all_connections() or you risk not
  // being prompted again for server certificates if you reconnect quickly. If
  // |callback| is non-NULL it will be executed on the UI thread after
  // completion.
  ///
  void(CEF_CALLBACK* clear_certificate_exceptions)(
      struct _cef_request_context_t* self,
      struct _cef_completion_callback_t* callback);

  ///
  // Clears all HTTP authentication credentials that were added as part of
  // handling GetAuthCredentials. If |callback| is non-NULL it will be executed
  // on the UI thread after completion.
  ///
  void(CEF_CALLBACK* clear_http_auth_credentials)(
      struct _cef_request_context_t* self,
      struct _cef_completion_callback_t* callback);

  ///
  // Clears all active and idle connections that Chromium currently has. This is
  // only recommended if you have released all other CEF objects but don't yet
  // want to call cef_shutdown(). If |callback| is non-NULL it will be executed
  // on the UI thread after completion.
  ///
  void(CEF_CALLBACK* close_all_connections)(
      struct _cef_request_context_t* self,
      struct _cef_completion_callback_t* callback);

  ///
  // Attempts to resolve |origin| to a list of associated IP addresses.
  // |callback| will be executed on the UI thread after completion.
  ///
  void(CEF_CALLBACK* resolve_host)(struct _cef_request_context_t* self,
                                   const cef_string_t* origin,
                                   struct _cef_resolve_callback_t* callback);

  ///
  // Load an extension.
  //
  // If extension resources will be read from disk using the default load
  // implementation then |root_directory| should be the absolute path to the
  // extension resources directory and |manifest| should be NULL. If extension
  // resources will be provided by the client (e.g. via cef_request_handler_t
  // and/or cef_extension_handler_t) then |root_directory| should be a path
  // component unique to the extension (if not absolute this will be internally
  // prefixed with the PK_DIR_RESOURCES path) and |manifest| should contain the
  // contents that would otherwise be read from the "manifest.json" file on
  // disk.
  //
  // The loaded extension will be accessible in all contexts sharing the same
  // storage (HasExtension returns true (1)). However, only the context on which
  // this function was called is considered the loader (DidLoadExtension returns
  // true (1)) and only the loader will receive cef_request_context_handler_t
  // callbacks for the extension.
  //
  // cef_extension_handler_t::OnExtensionLoaded will be called on load success
  // or cef_extension_handler_t::OnExtensionLoadFailed will be called on load
  // failure.
  //
  // If the extension specifies a background script via the "background"
  // manifest key then cef_extension_handler_t::OnBeforeBackgroundBrowser will
  // be called to create the background browser. See that function for
  // additional information about background scripts.
  //
  // For visible extension views the client application should evaluate the
  // manifest to determine the correct extension URL to load and then pass that
  // URL to the cef_browser_host_t::CreateBrowser* function after the extension
  // has loaded. For example, the client can look for the "browser_action"
  // manifest key as documented at
  // https://developer.chrome.com/extensions/browserAction. Extension URLs take
  // the form "chrome-extension://<extension_id>/<path>".
  //
  // Browsers that host extensions differ from normal browsers as follows:
  //  - Can access chrome.* JavaScript APIs if allowed by the manifest. Visit
  //    chrome://extensions-support for the list of extension APIs currently
  //    supported by CEF.
  //  - Main frame navigation to non-extension content is blocked.
  //  - Pinch-zooming is disabled.
  //  - CefBrowserHost::GetExtension returns the hosted extension.
  //  - CefBrowserHost::IsBackgroundHost returns true for background hosts.
  //
  // See https://developer.chrome.com/extensions for extension implementation
  // and usage documentation.
  ///
  void(CEF_CALLBACK* load_extension)(struct _cef_request_context_t* self,
                                     const cef_string_t* root_directory,
                                     struct _cef_dictionary_value_t* manifest,
                                     struct _cef_extension_handler_t* handler);

  ///
  // Returns true (1) if this context was used to load the extension identified
  // by |extension_id|. Other contexts sharing the same storage will also have
  // access to the extension (see HasExtension). This function must be called on
  // the browser process UI thread.
  ///
  int(CEF_CALLBACK* did_load_extension)(struct _cef_request_context_t* self,
                                        const cef_string_t* extension_id);

  ///
  // Returns true (1) if this context has access to the extension identified by
  // |extension_id|. This may not be the context that was used to load the
  // extension (see DidLoadExtension). This function must be called on the
  // browser process UI thread.
  ///
  int(CEF_CALLBACK* has_extension)(struct _cef_request_context_t* self,
                                   const cef_string_t* extension_id);

  ///
  // Retrieve the list of all extensions that this context has access to (see
  // HasExtension). |extension_ids| will be populated with the list of extension
  // ID values. Returns true (1) on success. This function must be called on the
  // browser process UI thread.
  ///
  int(CEF_CALLBACK* get_extensions)(struct _cef_request_context_t* self,
                                    cef_string_list_t extension_ids);

  ///
  // Returns the extension matching |extension_id| or NULL if no matching
  // extension is accessible in this context (see HasExtension). This function
  // must be called on the browser process UI thread.
  ///
  struct _cef_extension_t*(CEF_CALLBACK* get_extension)(
      struct _cef_request_context_t* self,
      const cef_string_t* extension_id);

  ///
  // Returns the MediaRouter object associated with this context.
  ///
  struct _cef_media_router_t*(CEF_CALLBACK* get_media_router)(
      struct _cef_request_context_t* self);
} cef_request_context_t;

///
// Returns the global context object.
///
CEF_EXPORT cef_request_context_t* cef_request_context_get_global_context();

///
// Creates a new context object with the specified |settings| and optional
// |handler|.
///
CEF_EXPORT cef_request_context_t* cef_request_context_create_context(
    const struct _cef_request_context_settings_t* settings,
    struct _cef_request_context_handler_t* handler);

///
// Creates a new context object that shares storage with |other| and uses an
// optional |handler|.
///
CEF_EXPORT cef_request_context_t* cef_create_context_shared(
    cef_request_context_t* other,
    struct _cef_request_context_handler_t* handler);

#ifdef __cplusplus
}
#endif

#endif  // CEF_INCLUDE_CAPI_CEF_REQUEST_CONTEXT_CAPI_H_

// Copyright (c) 2011 Marshall A. Greenblatt. All rights reserved.
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

#ifndef CEF_INCLUDE_INTERNAL_CEF_TIME_H_
#define CEF_INCLUDE_INTERNAL_CEF_TIME_H_
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <time.h>
#include "internal/cef_export.h"

///
// Time information. Values should always be in UTC.
///
typedef struct _cef_time_t {
  int year;          // Four or five digit year "2007" (1601 to 30827 on
                     //   Windows, 1970 to 2038 on 32-bit POSIX)
  int month;         // 1-based month (values 1 = January, etc.)
  int day_of_week;   // 0-based day of week (0 = Sunday, etc.)
  int day_of_month;  // 1-based day of month (1-31)
  int hour;          // Hour within the current day (0-23)
  int minute;        // Minute within the current hour (0-59)
  int second;        // Second within the current minute (0-59 plus leap
                     //   seconds which may take it up to 60).
  int millisecond;   // Milliseconds within the current second (0-999)
} cef_time_t;

///
// Converts cef_time_t to/from time_t. Returns true (1) on success and false (0)
// on failure.
///
CEF_EXPORT int cef_time_to_timet(const cef_time_t* cef_time, time_t* time);
CEF_EXPORT int cef_time_from_timet(time_t time, cef_time_t* cef_time);

///
// Converts cef_time_t to/from a double which is the number of seconds since
// epoch (Jan 1, 1970). Webkit uses this format to represent time. A value of 0
// means "not initialized". Returns true (1) on success and false (0) on
// failure.
///
CEF_EXPORT int cef_time_to_doublet(const cef_time_t* cef_time, double* time);
CEF_EXPORT int cef_time_from_doublet(double time, cef_time_t* cef_time);

///
// Retrieve the current system time.
//
CEF_EXPORT int cef_time_now(cef_time_t* cef_time);

///
// Retrieve the delta in milliseconds between two time values.
//
CEF_EXPORT int cef_time_delta(const cef_time_t* cef_time1,
                              const cef_time_t* cef_time2,
                              long long* delta);

#ifdef __cplusplus
}
#endif

#endif  // CEF_INCLUDE_INTERNAL_CEF_TIME_H_

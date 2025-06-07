#pragma once

#include <granny.h>

#include "../EterBase/StdAfx.h"
#include "../EterBase/Utils.h"
#include "../EterBase/Debug.h"
#include "../EterBase/Stl.h"

#include "../UserInterface/Locale_inc.h"

#include "Util.h"

// Armadillo nanomite protection
#if !defined(NANOBEGIN)
#	if defined(__BORLANDC__)
#		define NANOBEGIN __emit__(0xEB, 0x03, 0xD6, 0xD7, 0x01)
#		define NANOEND __emit__(0xEB, 0x03, 0xD6, 0xD7, 0x00)
#	else
#		define NANOBEGIN __asm _emit 0xEB __asm _emit 0x03 __asm _emit 0xD6 __asm _emit 0xD7 __asm _emit 0x01
#		define NANOEND __asm _emit 0xEB __asm _emit 0x03 __asm _emit 0xD6 __asm _emit 0xD7 __asm _emit 0x00
#	endif
#endif

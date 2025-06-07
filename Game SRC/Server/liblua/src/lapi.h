/*
* $Id: lapi.h,v 1.21 2002/03/04 21:29:41 roberto Exp $
* Auxiliary functions from Lua API
* See Copyright Notice in lua.h
*/

#ifndef __INC_LAPI_H__
#define __INC_LAPI_H__

#include "lobject.h"

void luaA_pushobject(lua_State* L, const TObject* o);

#endif // __INC_LAPI_H__

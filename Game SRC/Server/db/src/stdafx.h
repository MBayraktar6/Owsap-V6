#ifndef __INC_METIN_II_DB_SERV_STDAFX_H__
#define __INC_METIN_II_DB_SERV_STDAFX_H__

#include "../../libthecore/include/stdafx.h"

#ifndef __WIN32__
#include <semaphore.h>
#else
#define isdigit iswdigit
#define isspace iswspace
#endif

#include "../../common/length.h"
#include "../../common/tables.h"
#include "../../common/singleton.h"
#include "../../common/utils.h"
#include "../../common/stl.h"
#include "../../common/service.h"

#endif

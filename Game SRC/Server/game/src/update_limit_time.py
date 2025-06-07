#!/usr/local/bin/python

import time

desc = """#ifndef __INC_LIMIT_TIME_H__
#define __INC_LIMIT_TIME_H__

#define ENABLE_LIMIT_TIME
#define GLOBAL_LIMIT_TIME %dUL // %s

#endif // __INC_LIMIT_TIME_H__
"""

limitTime = time.mktime(time.localtime()) + 3600 * 24 * 180 * 2

open("limit_time.h", "w").write(desc % (limitTime, time.asctime(time.localtime(limitTime))))

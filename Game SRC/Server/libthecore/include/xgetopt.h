/*
* XGetopt.h Version 1.2
*
* Author: Hans Dietrich
* hdietrich2@hotmail.com
*
* This software is released into the public domain.
* You are free to use it in any way you like.
*
* This software is provided "as is" with no expressed
* or implied warranty. I accept no liability for any
* damage or loss of business that this software may cause.
*
*/

#ifndef __INC_LIBTHECORE_XGETOPT_H__
#define __INC_LIBTHECORE_XGETOPT_H__

extern int optind, opterr, optreset;
extern TCHAR* optarg;

int getopt(int argc, TCHAR* argv[], TCHAR* optstring);

#endif // __INC_LIBTHECORE_XGETOPT_H__

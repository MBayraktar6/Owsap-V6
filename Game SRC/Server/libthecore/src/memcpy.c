/*
* (c) Copyright 2000-2002 convergence integrated media GmbH.
* (c) Copyright 2002 convergence GmbH.
*
* All rights reserved.
*
* Written by Denis Oliver Kropp <dok@directfb.org>,
*	Andreas Hundt <andi@fischlustig.de> and
*	Sven Neumann <sven@convergence.de>.
*
* Fast memcpy code was taken from xine (see below).
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the
* Free Software Foundation, Inc., 59 Temple Place - Suite 330,
* Boston, MA 02111-1307, USA.
*/

/*
* Copyright (C) 2001 the xine project
*
* This file is part of xine, a unix video player.
*
* xine is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* xine is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
*
* These are the MMX/MMX2/SSE optimized versions of memcpy
*
* This code was adapted from Linux Kernel sources by Nick Kurshev to
* the mplayer program. (http://mplayer.sourceforge.net)
*
* Miguel Freitas split the #ifdefs into several specialized functions that
* are benchmarked at runtime by xine. Some original comments from Nick
* have been preserved documenting some MMX/SSE oddities.
* Also added kernel memcpy function that seems faster than glibc one.
*
*/

/*
* Original comments from mplayer (file: aclib.c) This part of code
* was taken by me from Linux-2.4.3 and slightly modified for MMX, MMX2,
* SSE instruction set. I have done it since linux uses page aligned
* blocks but mplayer uses weakly ordered data and original sources can
* not speedup them. Only using PREFETCHNTA and MOVNTQ together have
* effect!
*
* From IA-32 Intel Architecture Software Developer's Manual Volume 1,
*
* rder Number 245470:
* 10.4.6. Cacheability Control, Prefetch, and Memory Ordering Instructions"
*
* ata referenced by a program can be temporal (data will be used
* gain) or non-temporal (data will be referenced once and not reused
* n the immediate future). To make efficient use of the processor's
* aches, it is generally desirable to cache temporal data and not
* ache non-temporal data. Overloading the processor's caches with
* on-temporal data is sometimes referred to as "polluting the
* aches". The non-temporal data is written to memory with
* rite-Combining semantics.
*
* he PREFETCHh instructions permits a program to load data into the
* rocessor at a suggested cache level, so that it is closer to the
* rocessors load and store unit when it is needed. If the data is
* lready present in a level of the cache hierarchy that is closer to
* he processor, the PREFETCHh instruction will not result in any data
* ovement. But we should you PREFETCHNTA: Non-temporal data fetch
* ata into location close to the processor, minimizing cache
* ollution.
*
* he MOVNTQ (store quadword using non-temporal hint) instruction
* tores packed integer data from an MMX register to memory, using a
* on-temporal hint. The MOVNTPS (store packed single-precision
* loating-point values using non-temporal hint) instruction stores
* acked floating-point data from an XMM register to memory, using a
* on-temporal hint.
*
* he SFENCE (Store Fence) instruction controls write ordering by
* reating a fence for memory store operations. This instruction
* uarantees that the results of every store instruction that precedes
* he store fence in program order is globally visible before any
* tore instruction that follows the fence. The SFENCE instruction
* rovides an efficient way of ensuring ordering between procedures
* hat produce weakly-ordered data and procedures that consume that
* ata.
*
* f you have questions please contact with me: Nick Kurshev:
* ickols_k@mail.ru.
*/

/*
* mmx v.1 Note: Since we added alignment of destinition it speedups
* of memory copying on PentMMX, Celeron-1 and P2 upto 12% versus
* standard (non MMX-optimized) version.
* Note: on K6-2+ it speedups memory copying upto 25% and
* on K7 and P3 about 500% (5 times).
*/

/*
* Additional notes on gcc assembly and processors: [MF]
* prefetch is specific for AMD processors, the intel ones should be
* prefetch0, prefetch1, prefetch2 which are not recognized by my gcc.
* prefetchnta is supported both on athlon and pentium 3.
*
* therefore i will take off prefetchnta instructions from the mmx1
* version to avoid problems on pentium mmx and k6-2.
*
* quote of the day:
* "Using prefetches efficiently is more of an art than a science"
*/

#ifndef __WIN32__

#include <string.h>
void* (*thecore_memcpy) (void* to, const void* from, size_t len) = memcpy;

#endif

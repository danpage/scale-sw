/* Copyright (C) 2017 Daniel Page <dan@phoo.org>
 *
 * Use of this source code is restricted per the CC BY-SA license, a copy of
 * which can be found via http://creativecommons.org (and should be included 
 * as LICENSE.txt within the associated archive or repository).
 */

#ifndef __TARGET_H
#define __TARGET_H

#include <target/util.h>
#include <target/conv.h>
#include <target/mrz.h>

#include <gmp.h>

#if   ( CONF( POISONED_N, CID ) == 16 )
typedef uint16_t poisoned_t;
#elif ( CONF( POISONED_N, CID ) == 32 )
typedef uint32_t poisoned_t;
#elif ( CONF( POISONED_N, CID ) == 64 )
typedef uint64_t poisoned_t;
#endif

#endif

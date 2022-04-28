/* Copyright (C) 2017 Daniel Page <dan@phoo.org>
 *
 * Use of this source code is restricted per the CC BY-SA license, a copy of
 * which can be found via http://creativecommons.org (and should be included 
 * as LICENSE.txt within the associated archive or repository).
 */

#ifndef __TARGET_H
#define __TARGET_H

// ============================================================================

#include "util.h"
#include "conv.h"
#include  "des.h"

/* Dinero defines SIZEOF_VOIDP during configuration, but then doesn't include
 * it in config.h; it doesn't *seem* clear if this is intended or not, but it 
 * means that unless it's defined here before d4.h is included we get d4addr 
 * type of the wrong size (because the test whether SIZEOF_INT >= SIZEOF_VOID 
 * will always be true).  The important thing to note is that we're modelling 
 * a 32-bit processor, so need SIZEOF_VOIDP to match.
 */

#define SIZEOF_VOIDP ( 4 )

#include <d4.h>

// ----------------------------------------------------------------------------

#define CACHE_TRACE_SIZE_MIN ( 0       )
#define CACHE_TRACE_SIZE_INC ( 1 << 20 )

#define CACHE_TRACE_STAT_M   ( 0 )  /* miss   */
#define CACHE_TRACE_STAT_H   ( 1 )  /* hit    */
#define CACHE_TRACE_STAT_A   ( 2 )  /* access */

/* Cache parameters for attack target model a Pentium III "Katmai"
 *
 */

#define CACHE_MODEL_LN_ID       "Ln"
#define CACHE_MODEL_LN_CAPACITY ( 20 ) /* 2^20 = 1048576 = 1024 * 1024 */
#define CACHE_MODEL_LN_SUBWORDS (  0 ) 
#define CACHE_MODEL_LN_ASSOC    (  1 ) 
#define CACHE_MODEL_L2_ID       "L2"
#define CACHE_MODEL_L2_CAPACITY ( 19 ) /* 2^19 =  524288 =  512 * 1024 */
#define CACHE_MODEL_L2_SUBWORDS (  5 ) /* 2^5 =       32               */
#define CACHE_MODEL_L2_ASSOC    (  8 )
#define CACHE_MODEL_L1_ID       "L1"
#define CACHE_MODEL_L1_CAPACITY ( 14 ) /* 2^14 =   16384 =   16 * 1024 */
#define CACHE_MODEL_L1_SUBWORDS (  5 ) /* 2^5 =       32               */
#define CACHE_MODEL_L1_ASSOC    (  4 )

// ============================================================================

#endif







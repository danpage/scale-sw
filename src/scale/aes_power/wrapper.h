/* Copyright (C) 2017 Daniel Page <dan@phoo.org>
 *
 * Use of this source code is restricted per the CC BY-SA license, a copy of
 * which can be found via http://creativecommons.org (and should be included 
 * as LICENSE.txt within the associated archive or repository).
 */

#ifndef __WRAPPER_H
#define __WRAPPER_H

// ============================================================================

#include "util.h"
#include "conv.h"
#include  "aes.h"

#include "i8051.h"

// ----------------------------------------------------------------------------

extern void     aes_enc( uint8_t* c, const uint8_t* m, const uint8_t* k );
extern void     aes_dec( uint8_t* m, const uint8_t* c, const uint8_t* k );

extern void xts_aes_enc( uint8_t* c, const uint8_t* m, const uint8_t* k, const uint8_t* i, int j );
extern void xts_aes_dec( uint8_t* m, const uint8_t* c, const uint8_t* k, const uint8_t* i, int j );

extern void trace_rd( FILE* f );
extern void trace_wr( FILE* f );

// ============================================================================

#endif

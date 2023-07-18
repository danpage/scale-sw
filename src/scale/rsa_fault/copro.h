/* Copyright (C) 2017 Daniel Page <dan@phoo.org>
 *
 * Use of this source code is restricted per the CC BY-SA license, a copy of
 * which can be found via http://creativecommons.org (and should be included 
 * as LICENSE.txt within the associated archive or repository).
 */

#ifndef __COPRO_H
#define __COPRO_H

// ============================================================================

#include "util.h"
#include "conv.h"

#include <gmp.h>

// ----------------------------------------------------------------------------

#define COPRO_GPR       ( 16 )

#define COPRO_SPR_CYCLE (  0 )
#define COPRO_SPR_FAULT (  1 )

extern void copro_init();
extern void copro_fini();

extern int  copro_rdspr( int x        );
extern void copro_wrspr( int x, int y );

extern void copro_ld( int   r, const mpz_t x );
extern void copro_st( mpz_t r,       int   x );

extern int  copro_bit_len( int x               );
extern int  copro_bit_get( int x, int i        );
extern int  copro_bit_set( int x, int i, int y );

extern void copro_imm_add( int r, int x, int y );
extern void copro_imm_sub( int r, int x, int y );
extern void copro_imm_mul( int r, int x, int y );

extern void copro_mod_red( int r, int x,        int z );
extern void copro_mod_inv( int r, int x,        int z );

extern void copro_mod_add( int r, int x, int y, int z );
extern void copro_mod_sub( int r, int x, int y, int z );
extern void copro_mod_mul( int r, int x, int y, int z );

// ============================================================================

#endif

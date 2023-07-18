/* Copyright (C) 2017 Daniel Page <dan@phoo.org>
 *
 * Use of this source code is restricted per the CC BY-SA license, a copy of
 * which can be found via http://creativecommons.org (and should be included 
 * as LICENSE.txt within the associated archive or repository).
 */

#include "copro.h"

mpz_t copro_gpr[ COPRO_GPR ]; 

int copro_spr_cycle = 0;
int copro_spr_fault = 0;

void copro_cycle( mpz_t f ) {
  if( ( copro_spr_fault >= 0 ) && ( copro_spr_fault == copro_spr_cycle ) ) {
    mpz_combit( f, prng_32() % mpz_sizeinbase( f, 2 ) );
  }

  copro_spr_cycle++;
}

void copro_init() {
  for( int i = 0; i < COPRO_GPR; i++ ) {
    mpz_init_set_ui( copro_gpr[ i ], 0 );
  }
}
void copro_fini() {
  for( int i = 0; i < COPRO_GPR; i++ ) {
    mpz_clear( copro_gpr[ i ] );
  }
}

int  copro_rdspr( int x        ) {
  switch( x ) {
    case COPRO_SPR_CYCLE : {
      return copro_spr_cycle;
    }
    case COPRO_SPR_FAULT : {
      return copro_spr_fault;
    }
  }

  return -1;
}
void copro_wrspr( int x, int y ) {
  switch( x ) {
    case COPRO_SPR_CYCLE : {
      copro_spr_cycle = x; break;
    }
    case COPRO_SPR_FAULT : {
      copro_spr_fault = x; break;
    }
  }
}

void copro_ld( int   r, const mpz_t x ) {
  mpz_set( copro_gpr[ r ], x );
  copro_cycle( copro_gpr[ r ] );
}
void copro_st( mpz_t r,       int   x ) {
  mpz_set( r, copro_gpr[ x ] );
  copro_cycle(          ( r ) );
}

int  copro_bit_len( int x               ) {
  return mpz_sizeinbase( copro_gpr[ x ], 2 );
}
int  copro_bit_get( int x, int i        ) {
  return mpz_tstbit( copro_gpr[ x ], i );
}
int  copro_bit_set( int x, int i, int y ) {
  if( y == 0 ) {
    mpz_clrbit( copro_gpr[ x ], i );
  }
  else {
    mpz_setbit( copro_gpr[ x ], i );
  }
}

void copro_imm_add( int r, int x, int y        ) {
  mpz_add_ui( copro_gpr[ r ], copro_gpr[ x ],            y   ); copro_cycle( copro_gpr[ r ] );
}
void copro_imm_sub( int r, int x, int y        ) {
  mpz_sub_ui( copro_gpr[ r ], copro_gpr[ x ],            y   ); copro_cycle( copro_gpr[ r ] );
}
void copro_imm_mul( int r, int x, int y        ) {
  mpz_mul_ui( copro_gpr[ r ], copro_gpr[ x ],            y   ); copro_cycle( copro_gpr[ r ] );
}

void copro_mod_red( int r, int x,        int z ) {
  mpz_mod   ( copro_gpr[ r ], copro_gpr[ x ], copro_gpr[ z ] ); copro_cycle( copro_gpr[ r ] );
}
void copro_mod_inv( int r, int x,        int z ) {
  mpz_invert( copro_gpr[ r ], copro_gpr[ x ], copro_gpr[ z ] ); copro_cycle( copro_gpr[ r ] );
}
void copro_mod_add( int r, int x, int y, int z ) {
  mpz_add   ( copro_gpr[ r ], copro_gpr[ x ], copro_gpr[ y ] );
  mpz_mod   ( copro_gpr[ r ], copro_gpr[ r ], copro_gpr[ z ] ); copro_cycle( copro_gpr[ r ] );
}
void copro_mod_sub( int r, int x, int y, int z ) {
  mpz_sub   ( copro_gpr[ r ], copro_gpr[ x ], copro_gpr[ y ] );
  mpz_mod   ( copro_gpr[ r ], copro_gpr[ r ], copro_gpr[ z ] ); copro_cycle( copro_gpr[ r ] );
}
void copro_mod_mul( int r, int x, int y, int z ) {
  mpz_mul   ( copro_gpr[ r ], copro_gpr[ x ], copro_gpr[ y ] );
  mpz_mod   ( copro_gpr[ r ], copro_gpr[ r ], copro_gpr[ z ] ); copro_cycle( copro_gpr[ r ] );
}

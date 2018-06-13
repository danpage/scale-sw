/* Copyright (C) 2017 Daniel Page <dan@phoo.org>
 *
 * Use of this source code is restricted per the CC BY-SA license, a copy of
 * which can be found via http://creativecommons.org (and should be included 
 * as LICENSE.txt within the associated archive or repository).
 */

#ifndef __MRZ_H
#define __MRZ_H

#include "util.h"
#include "conv.h"

#include  <gmp.h>

typedef mp_limb_t mrz_t[ ( 1024 / GMP_LIMB_BITS ) + 2 ];

typedef struct __mrz_params_t {
      mrz_t     N; // modulus
     size_t   l_N; // modulus size in limbs

      mrz_t rho_0; // \rho^0 = ( b^{l_N} )^0 = 1 \pmod{N}
      mrz_t rho_1; // \rho^1 = ( b^{l_N} )^1     \pmod{N}
      mrz_t rho_2; // \rho^2 = ( b^{l_N} )^2     \pmod{N}

  mp_limb_t omega; // -N^{-1} \pmod{\rho}
} 
mrz_params_t;

#define MRZ_MUL(p,r,x,y,red_t,red_f){                               \
  mp_limb_t __t[ 2 * p->l_N + 2 ], *t = __t, u;                     \
                                                                    \
  memset( t, 0, ( 2 * p->l_N + 2 ) * SIZEOF( mp_limb_t ) );         \
                                                                    \
  for( int i = 0; i < p->l_N; i++ ) {                               \
    /* u = ( r_0 + y_i * x_0 ) * \omega \pmod{b} */                 \
    u = ( t[ 0 ] + y[ i ] * x[ 0 ] ) * p->omega;                    \
    /* r = r + ( y_i * x )                       */                 \
    t[ p->l_N + 1 ]  = mpn_addmul_1( t,    x, p->l_N + 1, y[ i ] ); \
    /* r = r + ( u   * N )                       */                 \
    t[ p->l_N + 1 ] += mpn_addmul_1( t, p->N, p->l_N + 1, u      ); \
    /* r = r / b                                 */                 \
    t++;                                                            \
  }                                                                 \
                                                                    \
  memcpy( r, t, ( 1 * p->l_N + 1 ) * SIZEOF( mp_limb_t ) );         \
                                                                    \
  if( r[ p->l_N ] || ( mpn_cmp( r, p->N, p->l_N ) >= 0 ) ) {        \
    mpn_sub_n( r, r, p->N, p->l_N + 1 ); red_t;                     \
  }                                                                 \
  else {                                                            \
                                         red_f;                     \
  }                                                                 \
}
#define MRZ_EXP(p,r,x,y,sqr,mul){                                   \
  mrz_t t;                                                          \
                                                                    \
  memcpy( t, p->rho_1, SIZEOF( mrz_t ) );                           \
                                                                    \
  for( int i = mpz_sizeinbase( y, 2 ) - 1; i >= 0; i-- ) {          \
    sqr; if( mpz_tstbit( y, i ) ) { mul; } ;                        \
  }                                                                 \
                                                                    \
  memcpy( r,        t, SIZEOF( mrz_t ) );                           \
}

// initialise/finalise parameters
extern void mrz_init( mrz_params_t* p, const mpz_t N );
extern void mrz_fini( mrz_params_t* p                );

// convert mpz_t into mrz_t
extern void mrz_import( const mrz_params_t* p, mrz_t r, const mpz_t x );
// convert mrz_t into mpz_t
extern void mrz_export( const mrz_params_t* p, mpz_t r, const mrz_t x );

// r = x + y \pmod{N}
extern void mrz_add( const mrz_params_t* p, mrz_t r, const mrz_t x, const mrz_t y );
// r = x - y \pmod{N}
extern void mrz_sub( const mrz_params_t* p, mrz_t r, const mrz_t x, const mrz_t y );

#endif

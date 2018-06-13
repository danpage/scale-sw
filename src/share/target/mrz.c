/* Copyright (C) 2017 Daniel Page <dan@phoo.org>
 *
 * Use of this source code is restricted per the CC BY-SA license, a copy of
 * which can be found via http://creativecommons.org (and should be included 
 * as LICENSE.txt within the associated archive or repository).
 */

#include "mrz.h"

void mrz_init( mrz_params_t* p, const mpz_t N ) {
  memset( p, 0, SIZEOF( mrz_params_t ) );

  // initialise N, and l_N = |N|

  mpz_export( p->N, &p->l_N, -1, SIZEOF( mp_limb_t ), -1, 0, N );

  // compute \rho^i \pmod{N}, for i \in { 0, 1, 2 }

  p->rho_0[ 0 ] = 1;
  p->rho_1[ 0 ] = 1;

  for( int i = 0; i < ( p->l_N * BITSOF( mp_limb_t ) ); i++ ) {
    mrz_add( p, p->rho_1, p->rho_1, p->rho_1 );
  }

  memcpy( p->rho_2, p->rho_1, SIZEOF( mrz_t ) );

  for( int i = 0; i < ( p->l_N * BITSOF( mp_limb_t ) ); i++ ) {
    mrz_add( p, p->rho_2, p->rho_2, p->rho_2 );
  }

  // compute \omega = -N^{-1} \pmod{b}

  p->omega =         1;

  for( int i = 1; i < (          BITSOF( mp_limb_t ) ); i++ ) {
    p->omega = p->omega * p->omega * p->N[ 0 ];
  }

  p->omega = -p->omega;
}

void mrz_fini( mrz_params_t* p ) {

}

void mrz_import( const mrz_params_t* p, mrz_t r, const mpz_t x ) {
  memset( r, 0, SIZEOF( mrz_t ) );

  mpz_export( r, NULL, -1, SIZEOF( mp_limb_t ), -1, 0, x );
}

void mrz_export( const mrz_params_t* p, mpz_t r, const mrz_t x ) {
  int n = p->l_N;

  while( ( x[ n - 1 ] == 0 ) && ( n > 1 ) ) {
    n--;
  }

  mpz_import( r,    n, -1, SIZEOF( mp_limb_t ), -1, 0, x );
}

void mrz_add( const mrz_params_t* p, mrz_t r, const mrz_t x, const mrz_t y ) {
  r[ p->l_N ] = mpn_add_n( r, x, y, p->l_N );

  if( r[ p->l_N ] || ( mpn_cmp( r, p->N, p->l_N ) >= 0 ) ) {
    mpn_sub_n( r, r, p->N, p->l_N + 1 );
  }
}

void mrz_sub( const mrz_params_t* p, mrz_t r, const mrz_t x, const mrz_t y ) {
  if( mpn_cmp( y, x, p->l_N ) > 0 ) {
    mpn_sub_n( r,    y, x, p->l_N );
    mpn_sub_n( r, p->N, r, p->l_N ); 
  }
  else {
    mpn_sub_n( r,    x, y, p->l_N );
  }
}

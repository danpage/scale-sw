/* Copyright (C) 2017 Daniel Page <dan@phoo.org>
 *
 * Use of this source code is restricted per the CC BY-SA license, a copy of
 * which can be found via http://creativecommons.org (and should be included 
 * as LICENSE.txt within the associated archive or repository).
 */

#include "target.h"

int scan( const poisoned_t* x, int n, poisoned_t y ) {
  for( int i = 0; i < n; i++ ) {
    if( x[ i ] == y ) {
      return i;
    }
  }

  return -1;
}

void mrz_mul( const mrz_params_t* p, mrz_t r, const mrz_t x, const mrz_t y ) {
  int          l = p->l_N * ( SIZEOF( mp_limb_t ) / SIZEOF( poisoned_t ) );

  poisoned_t* xp = ( poisoned_t* )( x );
  int         xi = scan( xp, l, HEX( CONF( POISONED_X, CID ) ) );
  poisoned_t* yp = ( poisoned_t* )( y );
  int         yi = scan( yp, l, HEX( CONF( POISONED_Y, CID ) ) );

  if( ( xi >= 0 ) && ( yi >= 0 ) ) {
    xp[ xi ] = yp[ yi ] = 0;
  }

  MRZ_MUL( p, r, x, y, /* empty */, /* empty */ );

  return;
}

void mrz_exp( const mrz_params_t* p, mrz_t r, const mrz_t x, const mpz_t y ) {
  MRZ_EXP( p, r, x, y, mrz_mul( p, t, t, t ),
                       mrz_mul( p, t, t, x ) );

  return;
}

int main( int argc, char* argv[] ) {
  uint8_t __N[] = { USER( N_DATA, CID ) };
  uint8_t __d[] = { USER( D_DATA, CID ) };

  mpz_t N, d, c, m;

  #if   CONF( TARGET_T, CID ) &&  CONF( OBFUSCATE )
  unmask( __N,    USER( N_SIZE, CID ), USER( N_MASK, CID ) );
  unmask( __d,    USER( D_SIZE, CID ), USER( D_MASK, CID ) );
  #elif CONF( TARGET_R, CID ) || !CONF( OBFUSCATE )
  memset( __N, 0, USER( N_SIZE, CID )                      );
  memset( __d, 0, USER( D_SIZE, CID )                      );
  #endif

  mpz_inits( N, d, c, m, NULL );

  while( true ) {
    // 1. consume input

    CONSUME( gmp_scanf( "%ZX", c ), 1 );

    #if   CONF( TARGET_T, CID )
    mpz_import( N, USER( N_SIZE, CID ), -1, SIZEOF( uint8_t ), -1, 0, __N );
    mpz_import( d, USER( D_SIZE, CID ), -1, SIZEOF( uint8_t ), -1, 0, __d );
    #elif CONF( TARGET_R, CID )
    CONSUME( gmp_scanf( "%ZX", N ), 1 );
    CONSUME( gmp_scanf( "%ZX", d ), 1 );
    #endif

    // 2. execute operation

    mrz_params_t p; mrz_t t;

    mrz_init( &p, N );
    mrz_import( &p, t, c );

    mrz_mul( &p, t, t, p.rho_2 );
    mrz_exp( &p, t, t, d       );
    mrz_mul( &p, t, t, p.rho_0 );

    mrz_export( &p, m, t );
    mrz_fini( &p    );

    #if CONF( DEBUG )
    fprintf( stderr, "N = " ); gmp_fprintf( stderr, "%ZX\n", N );
    fprintf( stderr, "d = " ); gmp_fprintf( stderr, "%ZX\n", d );
    fprintf( stderr, "c = " ); gmp_fprintf( stderr, "%ZX\n", c );
    fprintf( stderr, "m = " ); gmp_fprintf( stderr, "%ZX\n", m );
    #endif

    // 3. produce output
    
                               gmp_fprintf( stdout, "%ZX\n", m ); 

    // 4. flush streams

    fflush( stdout );
    fflush( stderr );
  }

  mpz_clears( N, d, c, m, NULL );

  return 0;
}

/* Copyright (C) 2017 Daniel Page <dan@phoo.org>
 *
 * Use of this source code is restricted per the CC BY-SA license, a copy of
 * which can be found via http://creativecommons.org (and should be included 
 * as LICENSE.txt within the associated archive or repository).
 */

#include "target.h"

/*
Using Table 2 from Koc et al. verbatim, we find that for a modulus N with 
s limbs, the integrated integer multiplication and reduction (steps 1 and 
2) followed by the conditional subtraction (step 3) take

    +---------------+---------------+---------------+   
    | Step 1+2      | Step     3    | Step 1+2+3    |
----+---------------+---------------+---------------+   
mul | 2s^2 + 1s     |               | 2s^2 + 1s     |
add | 4s^2 + 2s     |        2s + 2 | 4s^2 + 4s + 2 |
ldr | 6s^2 + 5s     |        2s + 2 | 6s^2 + 7s + 2 |
str | 2s^2 + 4s     |        1s + 1 | 2s^2 + 5s + 1 |
----+---------------+---------------+---------------+   

assuming an equal, say 1-cycle latency for each instruction type.
*/

int mrz_mul( const mrz_params_t* p, mrz_t r, const mrz_t x, const mrz_t y ) {
  bool red; int s = p->l_N;

  MRZ_MUL( p, r, x, y, red = true, red = false );

  if( red ) {
    return ( 2*s*s + 1*s     ) + 
           ( 4*s*s + 4*s + 2 ) + 
           ( 6*s*s + 7*s + 2 ) + 
           ( 2*s*s + 5*s + 1 ) ;
  }
  else {
    return ( 2*s*s + 1*s     ) + 
           ( 4*s*s + 2*s     ) + 
           ( 6*s*s + 5*s     ) + 
           ( 2*s*s + 4*s     ) ;
  }

  return -1;
}

int mrz_exp( const mrz_params_t* p, mrz_t r, const mrz_t x, const mpz_t y ) {
  int cycles = 0;

  MRZ_EXP( p, r, x, y, cycles += mrz_mul( p, t, t, t ), 
                       cycles += mrz_mul( p, t, t, x ) );

  return cycles;
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
    int Lambda;

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

    Lambda  = mrz_mul( &p, t, t, p.rho_2 );
    Lambda += mrz_exp( &p, t, t, d       );
    Lambda += mrz_mul( &p, t, t, p.rho_0 );

    mrz_export( &p, m, t );
    mrz_fini( &p    );

    #if ( ( CONF( NOISE_MAX, CID ) - CONF( NOISE_MIN, CID ) ) > 0 )
    Lambda += CONF( NOISE_MIN, CID ) + ( prng_32() % ( CONF( NOISE_MAX, CID ) - CONF( NOISE_MIN, CID ) ) );
    #endif

    #if CONF( DEBUG )
    fprintf( stderr, "N = " ); gmp_fprintf( stderr, "%ZX\n", N      );
    fprintf( stderr, "d = " ); gmp_fprintf( stderr, "%ZX\n", d      );
    fprintf( stderr, "c = " ); gmp_fprintf( stderr, "%ZX\n", c      );
    fprintf( stderr, "m = " ); gmp_fprintf( stderr, "%ZX\n", m      );
    #endif

    // 4. produce output

                               gmp_fprintf( stdout,  "%d\n", Lambda );
                               gmp_fprintf( stdout, "%ZX\n", m      );

    // 5. flush streams

    fflush( stdout );
    fflush( stderr );
  }

  mpz_clears( N, d, c, m, NULL );

  return 0;
}

/* Copyright (C) 2017 Daniel Page <dan@phoo.org>
 *
 * Use of this source code is restricted per the CC BY-SA license, a copy of
 * which can be found via http://creativecommons.org (and should be included 
 * as LICENSE.txt within the associated archive or repository).
 */

#include "target.h"

/* TODO: 
 * 
 * 1. compute 
 *    
 *    sigma = RSA.Sig( ( N, p, q, d ), m )
 *          = m^d \pmod{N}
 * 
 *    inducing a fault in cycle delta if delta > 0,
 * 
 * 2. return number of elapsed cycles
 */

int sign( mpz_t sigma, const mpz_t N, const mpz_t p, const mpz_t q, const mpz_t d, const mpz_t m, int delta ) {
  return 0;
}

int main( int argc, char* argv[] ) {
  uint8_t __N[] = { USER( N_DATA, CID ) };
  uint8_t __p[] = { USER( P_DATA, CID ) };
  uint8_t __q[] = { USER( Q_DATA, CID ) };
  uint8_t __d[] = { USER( D_DATA, CID ) };

  mpz_t N, p, q, d, m, sigma;

  mpz_inits( N, p, q, d, m, sigma, NULL );

  #if   CONF( TARGET_D, CID ) &&  CONF( OBFUSCATE )
  unmask( __N,    USER( N_SIZE, CID ), USER( N_MASK, CID ) ); 
  unmask( __p,    USER( P_SIZE, CID ), USER( P_MASK, CID ) ); 
  unmask( __q,    USER( Q_SIZE, CID ), USER( Q_MASK, CID ) ); 
  unmask( __d,    USER( D_SIZE, CID ), USER( D_MASK, CID ) ); 
  #elif CONF( TARGET_R, CID ) || !CONF( OBFUSCATE )
  memset( __N, 0, USER( N_SIZE, CID )                      );
  memset( __p, 0, USER( P_SIZE, CID )                      );
  memset( __q, 0, USER( Q_SIZE, CID )                      );
  memset( __d, 0, USER( D_SIZE, CID )                      );
  #endif

  while( true ) {
    int lambda, delta;

    // 1. consume input

    CONSUME( gmp_scanf(  "%d", &delta ), 1 );
    CONSUME( gmp_scanf( "%ZX",  m     ), 1 );

    #if   CONF( TARGET_D, CID )
    mpz_import( N, USER( N_SIZE, CID ), -1, SIZEOF( uint8_t ), -1, 0, __N );
    mpz_import( p, USER( P_SIZE, CID ), -1, SIZEOF( uint8_t ), -1, 0, __p );
    mpz_import( q, USER( Q_SIZE, CID ), -1, SIZEOF( uint8_t ), -1, 0, __q );
    mpz_import( d, USER( D_SIZE, CID ), -1, SIZEOF( uint8_t ), -1, 0, __d );
    #elif CONF( TARGET_R, CID )
    CONSUME( gmp_scanf( "%ZX",  N     ), 1 );
    CONSUME( gmp_scanf( "%ZX",  p     ), 1 );
    CONSUME( gmp_scanf( "%ZX",  q     ), 1 );
    CONSUME( gmp_scanf( "%ZX",  d     ), 1 );
    #endif

    // 2. execute operation

    lambda = sign( sigma, N, p, q, d, m, delta );

    #if CONF( DEBUG )
    fprintf( stderr, "delta = " ); gmp_fprintf( stderr,  "%d\n", delta  );

    fprintf( stderr, "N     = " ); gmp_fprintf( stderr, "%ZX\n", N      );
    fprintf( stderr, "p     = " ); gmp_fprintf( stderr, "%ZX\n", p      );
    fprintf( stderr, "q     = " ); gmp_fprintf( stderr, "%ZX\n", q      );
    fprintf( stderr, "d     = " ); gmp_fprintf( stderr, "%ZX\n", d      );

    fprintf( stderr, "m     = " ); gmp_fprintf( stderr, "%ZX\n", m      );
    fprintf( stderr, "sigma = " ); gmp_fprintf( stderr, "%ZX\n", sigma  );
    #endif

    // 3. produce output

                                   gmp_fprintf( stdout,  "%d\n", lambda );
                                   gmp_fprintf( stderr, "%ZX\n", sigma  );

    // 4. flush streams

    fflush( stdout );
    fflush( stderr );
  }

  mpz_clears( N, p, q, d, m, sigma, NULL );

  return 0;
}

/* Copyright (C) 2017 Daniel Page <dan@phoo.org>
 *
 * Use of this source code is restricted per the CC BY-SA license, a copy of
 * which can be found via http://creativecommons.org (and should be included 
 * as LICENSE.txt within the associated archive or repository).
 */

#include "target.h"

int sign( mpz_t sigma, const mpz_t N, const mpz_t p, const mpz_t q, const mpz_t d, const mpz_t m, int Delta ) {
  // initialise
  copro_init();
  copro_wrspr( COPRO_SPR_CYCLE,     0 );
  copro_wrspr( COPRO_SPR_FAULT, Delta );
  // transfer  input
  copro_ld( 0, N );
  copro_ld( 1, p );
  copro_ld( 2, q );
  copro_ld( 3, d );
  copro_ld( 4, m );
  // compute GPR[ 5 ] = 1 / p \pmod{q}
      copro_mod_inv( 5, 1,    2 );
  // compute GPR[ 6 ] = 1 / q \pmod{p}
      copro_mod_inv( 6, 2,    1 );
  // compute GPR[ 7 ] = m^{d \bmod p-1} \pmod{p}
      copro_imm_sub( 9, 1, 1    );
      copro_mod_red( 9, 3,    9 );
      copro_mod_red( 7, 4,    1 );
  for( int i = copro_bit_len( 9 ) - 2; i >= 0; i-- ) {
      copro_mod_mul( 7, 7, 7, 1 );
    if( copro_bit_get( 9, i ) ) {
      copro_mod_mul( 7, 7, 4, 1 );
    }
  }
  // compute GPR[ 8 ] = m^{d \bmod q-1} \pmod{q}
      copro_imm_sub( 9, 2, 1    );
      copro_mod_red( 9, 3,    9 );
      copro_mod_red( 8, 4,    2 );
  for( int i = copro_bit_len( 9 ) - 2; i >= 0; i-- ) {
      copro_mod_mul( 8, 8, 8, 2 );
    if( copro_bit_get( 9, i ) ) {
      copro_mod_mul( 8, 8, 4, 2 );
    }
  }
  // compute GPR[ 7 ] = t_0 = ( ( m^{d \bmod p-1} ) \cdot ( 1 / q ) \cdot ( q ) ) \pmod{N}
      copro_mod_mul( 7, 7, 6, 0 );
      copro_mod_mul( 7, 7, 2, 0 );
  // compute GPR[ 8 ] = t_1 = ( ( m^{d \bmod q-1} ) \cdot ( 1 / p ) \cdot ( p ) ) \pmod{N}
      copro_mod_mul( 8, 8, 5, 0 );
      copro_mod_mul( 8, 8, 1, 0 );
  // compute GPR[ 7 ] = t_0 + t_1 \pmod{N}
      copro_mod_add( 7, 7, 8, 0 );
  // transfer output
  copro_st( sigma, 7 );
  //   finalise
  copro_fini();

  return copro_rdspr( COPRO_SPR_CYCLE );
}

int main( int argc, char* argv[] ) {
  uint8_t __N[] = { USER( N_DATA, CID ) };
  uint8_t __p[] = { USER( P_DATA, CID ) };
  uint8_t __q[] = { USER( Q_DATA, CID ) };
  uint8_t __d[] = { USER( D_DATA, CID ) };

  mpz_t N, p, q, d, m, sigma;

  mpz_inits( N, p, q, d, m, sigma, NULL );
  copro_init();

  #if   CONF( TARGET_T, CID ) &&  CONF( OBFUSCATE )
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
    int Lambda, Delta;

    // 1. consume input

    CONSUME( gmp_scanf(  "%d", &Delta ), 1 );
    CONSUME( gmp_scanf( "%ZX",  m     ), 1 );

    #if   CONF( TARGET_T, CID )
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

    Lambda = sign( sigma, N, p, q, d, m, Delta );

    #if CONF( DEBUG )
    fprintf( stderr, "Lambda = " ); gmp_fprintf( stderr,  "%d\n", Lambda );
    fprintf( stderr, "Delta  = " ); gmp_fprintf( stderr,  "%d\n", Delta  );

    fprintf( stderr, "N      = " ); gmp_fprintf( stderr, "%ZX\n", N      );
    fprintf( stderr, "p      = " ); gmp_fprintf( stderr, "%ZX\n", p      );
    fprintf( stderr, "q      = " ); gmp_fprintf( stderr, "%ZX\n", q      );
    fprintf( stderr, "d      = " ); gmp_fprintf( stderr, "%ZX\n", d      );

    fprintf( stderr, "m      = " ); gmp_fprintf( stderr, "%ZX\n", m      );
    fprintf( stderr, "sigma  = " ); gmp_fprintf( stderr, "%ZX\n", sigma  );
    #endif

    // 3. produce output

                                    gmp_fprintf( stdout,  "%d\n", Lambda );
                                    gmp_fprintf( stdout, "%ZX\n", sigma  );

    // 4. flush streams

    fflush( stdout );
    fflush( stderr );
  }

  mpz_clears( N, p, q, d, m, sigma, NULL );

  return 0;
}

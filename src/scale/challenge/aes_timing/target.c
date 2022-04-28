/* Copyright (C) 2017 Daniel Page <dan@phoo.org>
 *
 * Use of this source code is restricted per the CC BY-SA license, a copy of
 * which can be found via http://creativecommons.org (and should be included 
 * as LICENSE.txt within the associated archive or repository).
 */

#include "target.h"

int aes_enc( uint8_t* c, const uint8_t* m, const uint8_t* k ) {
  uint8_t s[ 4 * AES_128_NB ], rk[ 4 * AES_128_NK ], rc = 0x01;

  U8_TO_U8_N( rk, k );
  U8_TO_U8_N(  s, m );

  int cycles = 1000;

  //      1 initial round
    aes_enc_key( s, rk );
  // Nr - 1 normal  rounds
  for( int r = 1; r < AES_128_NR; r++ ) {
    aes_enc_sub( s     );
    aes_enc_row( s     );

    for( int i = 0; i < ( 4 * AES_128_NB ); i++ ) {
      cycles += ( s[ i ] & 0x80 ) ? 2 : 1;
    }

    aes_enc_mix( s     );
    rc = aes_enc_keyexp_step( rk, rk, rc );
    aes_enc_key( s, rk );
  }
  //      1 final   round
    aes_enc_sub( s     );
    aes_enc_row( s     );
    rc = aes_enc_keyexp_step( rk, rk, rc );
    aes_enc_key( s, rk );

  U8_TO_U8_N(  c, s );

  return cycles;
}

int main( int argc, char* argv[] ) {
  uint8_t c[ 4 * AES_128_NB ], m[ 4 * AES_128_NB ], k[] = { USER( K_DATA, CID ) };

  #if   CONF( TARGET_D, CID ) &&  CONF( OBFUSCATE )
  unmask( k,    USER( K_SIZE, CID ), USER( K_MASK, CID ) );
  #elif CONF( TARGET_R, CID ) || !CONF( OBFUSCATE )
  memset( k, 0, USER( K_SIZE, CID )                      );
  #endif

  while( true ) {
    // 1. consume input

    CONSUME( octetstr_rd( stdin, m, 4 * AES_128_NB ), 4 * AES_128_NB );

    #if CONF( TARGET_R, CID )
    CONSUME( octetstr_rd( stdin, k, 4 * AES_128_NK ), 4 * AES_128_NK );
    #endif

    // 2. execute operation
  
    int lambda = aes_enc( c, m, k );
 
    #if ( ( CONF( NOISE_MAX, CID ) - CONF( NOISE_MIN, CID ) ) > 0 )
    lambda += CONF( NOISE_MIN, CID ) + ( prng_32() % ( CONF( NOISE_MAX, CID ) - CONF( NOISE_MIN, CID ) ) );
    #endif

    #if CONF( DEBUG )
    fprintf( stderr, "c = " ); octetstr_wr( stderr, c, 4 * AES_128_NB );
    fprintf( stderr, "m = " ); octetstr_wr( stderr, m, 4 * AES_128_NB );
    fprintf( stderr, "k = " ); octetstr_wr( stderr, k, 4 * AES_128_NK );
    #endif

    // 3. produce output
    
    fprintf( stdout,"%d\n", lambda );

                               octetstr_wr( stdout, c, 4 * AES_128_NB ); 
   
    // 4. flush streams

    fflush( stdout );
    fflush( stderr );
  }
  
  return 0;
}

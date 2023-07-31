/* Copyright (C) 2017 Daniel Page <dan@phoo.org>
 *
 * Use of this source code is restricted per the CC BY-SA license, a copy of
 * which can be found via http://creativecommons.org (and should be included 
 * as LICENSE.txt within the associated archive or repository).
 */

#include "target_ecb.h"

int main( int argc, char* argv[] ) {
  uint8_t c[ 4 * AES_128_NB ], m[ 4 * AES_128_NB ], k[] = { USER( K_DATA, CID ) };

  #if   CONF( TARGET_T, CID ) &&  CONF( OBFUSCATE )
  unmask( k,    USER( K_SIZE, CID ), USER( K_MASK, CID ) );
  #elif CONF( TARGET_R, CID ) || !CONF( OBFUSCATE )
  memset( k, 0, USER( K_SIZE, CID )                      );
  #endif
  
  i8051_init();  

  while( true ) {
    // 1. consume input

    #if   CONF( VERSION, CID ) == 0
    CONSUME( octetstr_rd( stdin, m, 4 * AES_128_NB ), 4 * AES_128_NB );
    #elif CONF( VERSION, CID ) == 1
    CONSUME( octetstr_rd( stdin, c, 4 * AES_128_NB ), 4 * AES_128_NB );
    #endif

    #if CONF( TARGET_R, CID )
    CONSUME( octetstr_rd( stdin, k, 4 * AES_128_NK ), 4 * AES_128_NK );
    #endif

    // 2. execute operation

    #if   CONF( VERSION, CID ) == 0
    i8051_init(); aes_enc( c, m, k );
    #elif CONF( VERSION, CID ) == 1
    i8051_init(); aes_dec( m, c, k );
    #endif

    #if CONF( DEBUG )
    fprintf( stderr, "c = " ); octetstr_wr( stderr, c, 4 * AES_128_NB );
    fprintf( stderr, "m = " ); octetstr_wr( stderr, m, 4 * AES_128_NB );
    fprintf( stderr, "k = " ); octetstr_wr( stderr, k, 4 * AES_128_NK );
    #endif

    // 3. produce output
                               
    #if   CONF( VERSION, CID ) == 0
    trace_wr( stdout );        octetstr_wr( stdout, c, 4 * AES_128_NB );
    #elif CONF( VERSION, CID ) == 1
    trace_wr( stdout );        octetstr_wr( stdout, m, 4 * AES_128_NB );
    #endif

    // 4. flush streams

    fflush( stdout );
    fflush( stderr );
  }

  i8051_fini();  

  return 0;
}

/* Copyright (C) 2017 Daniel Page <dan@phoo.org>
 *
 * Use of this source code is restricted per the CC BY-SA license, a copy of
 * which can be found via http://creativecommons.org (and should be included 
 * as LICENSE.txt within the associated archive or repository).
 */

#include "target_xts.h"

int main( int argc, char* argv[] ) {
  uint8_t c[ 4 * AES_128_NB ], m[ 4 * AES_128_NB ], k[] = { USER( K_DATA, CID ) };

  #if   CONF( TARGET_D, CID ) &&  CONF( OBFUSCATE )
  unmask( k,    USER( K_SIZE, CID ), USER( K_MASK, CID ) );
  #elif CONF( TARGET_R, CID ) || !CONF( OBFUSCATE )
  memset( k, 0, USER( K_SIZE, CID )                      );
  #endif

  OpenSSL_add_all_algorithms();

  while( true ) { 
    // 1. consume input

    int j;

    CONSUME( fscanf( stdin, "%d\n", &j ), 1 );

    uint8_t i[ 4 * AES_128_NB ];

    CONSUME( octetstr_rd( stdin, i, 1 * 4 * AES_128_NB ), 1 * 4 * AES_128_NB );

    #if CONF( TARGET_R, CID )
    CONSUME( octetstr_rd( stdin, c, 1 * 4 * AES_128_NB ), 1 * 4 * AES_128_NB );
    CONSUME( octetstr_rd( stdin, k, 2 * 4 * AES_128_NK ), 2 * 4 * AES_128_NK );
    #endif
        
    // 2. execute operation
 
    uint8_t t[ SHA_DIGEST_LENGTH ];

    #if CONF( TARGET_D, CID )    
    if( ( j < 0 ) || ( j >= 256 ) || !memeqz( i + 3, 4 * AES_128_NB - 3 ) ) {
      memset( c, 0, 1 * 4 * AES_128_NB * SIZEOF( uint8_t ) );
    }
    else {
      SHA_CTX ctx;
    
      SHA1_Init  ( &ctx );
      SHA1_Update( &ctx, &j,                  SIZEOF(  int    ) );
      SHA1_Update( &ctx, &i, 4 * AES_128_NB * SIZEOF( uint8_t ) );
      SHA1_Final ( t, &ctx );

      memcpy( c, t, 1 * 4 * AES_128_NB * SIZEOF( uint8_t ) );
    }
    #endif

    i8051_init(); xts_aes_dec( m, c, k, i, j );
   
    #if CONF( DEBUG )
    fprintf( stderr, "j = %d\n", j );    

    fprintf( stderr, "i = " ); octetstr_wr( stderr, i, 1 * 4 * AES_128_NB );    
    fprintf( stderr, "c = " ); octetstr_wr( stderr, c, 1 * 4 * AES_128_NB );
    fprintf( stderr, "m = " ); octetstr_wr( stderr, m, 1 * 4 * AES_128_NB );
    fprintf( stderr, "k = " ); octetstr_wr( stderr, k, 2 * 4 * AES_128_NK );
    #endif
  
    // 3. produce output

    trace_wr( stdout );        octetstr_wr( stdout, m, 1 * 4 * AES_128_NB );

    // 4. flush streams

    fflush( stdout );
    fflush( stderr );
  }

  i8051_fini();  

  return 0;
}

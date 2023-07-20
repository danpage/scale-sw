/* Copyright (C) 2017 Daniel Page <dan@phoo.org>
 *
 * Use of this source code is restricted per the CC BY-SA license, a copy of
 * which can be found via http://creativecommons.org (and should be included 
 * as LICENSE.txt within the associated archive or repository).
 */

#include "target.h"

fault_t Delta;

int fault_rd( FILE* fd ) {
  char T[ BUFFER ], *Tp = fgets( T, BUFFER, fd ); 

  if( Tp == NULL ) {
                          return 0;
  }

  Tp = strip( Tp );

  if( *Tp == '\x00' ) {
    Delta.enable = false; return 1;
  }

  if( 5 != sscanf( Tp, "%d,%d,%d,%d,%d", &Delta.round,
                                         &Delta.function,
                                         &Delta.priority,
                                         &Delta.row,
                                         &Delta.col ) ) {
    Delta.enable = false; return 0;
  }
  else {
    Delta.enable =  true; return 1;
  }
}

void aes_enc_step( uint8_t* s, uint8_t* rk, int r, int f ) {
  if( ( Delta.enable                          ) &&
      ( Delta.round    == r                   ) &&
      ( Delta.function == f                   ) &&
      ( Delta.priority == FAULT_PRIORITY_PRE  ) ) { 
    uint8_t t;

    #if CONF( ALLOW_ZERO, CID )
         t = prng() & 0xFF;
    #else
    do { t = prng() & 0xFF; } while( t == 0 );
    #endif 

    s[ ( Delta.col * 4 ) + Delta.row ] ^= t;
  }

  switch( f ) {
    case FAULT_FUNCTION_KEY : {
      aes_enc_key( s, rk ); 
      break;
    }
    case FAULT_FUNCTION_SUB : {
      aes_enc_sub( s     ); 
      break;
    }
    case FAULT_FUNCTION_ROW : {
      aes_enc_row( s     ); 
      break;
    }
    case FAULT_FUNCTION_MIX : {
      aes_enc_mix( s     ); 
      break;
    }
  }

  if( ( Delta.enable                          ) &&
      ( Delta.round    == r                   ) &&
      ( Delta.function == f                   ) &&
      ( Delta.priority == FAULT_PRIORITY_POST ) ) { 
    uint8_t t;

    #if CONF( ALLOW_ZERO, CID )
         t = prng() & 0xFF;
    #else
    do { t = prng() & 0xFF; } while( t == 0 );
    #endif 

    s[ ( Delta.col * 4 ) + Delta.row ] ^= t;
  }

  return;
}

void aes_enc( uint8_t* c, const uint8_t* m, const uint8_t* k ) {
  uint8_t s[ 4 * AES_128_NB ], rk[ 4 * AES_128_NK ], rc = 0x01;

  U8_TO_U8_N( rk, k );
  U8_TO_U8_N(  s, m );

  //      1 initial round
    aes_enc_step( s, rk,  0, FAULT_FUNCTION_KEY );
  // Nr - 1 normal  rounds
  for( int r = 1; r < AES_128_NR; r++ ) {
    aes_enc_step( s, rk,  r, FAULT_FUNCTION_SUB );
    aes_enc_step( s, rk,  r, FAULT_FUNCTION_ROW );
    aes_enc_step( s, rk,  r, FAULT_FUNCTION_MIX );
    rc = aes_enc_keyexp_step( rk, rk, rc );
    aes_enc_step( s, rk,  r, FAULT_FUNCTION_KEY );
  }
  //      1 final   round
    aes_enc_step( s, rk, 10, FAULT_FUNCTION_SUB );
    aes_enc_step( s, rk, 10, FAULT_FUNCTION_ROW );
    rc = aes_enc_keyexp_step( rk, rk, rc );
    aes_enc_step( s, rk, 10, FAULT_FUNCTION_KEY );

  U8_TO_U8_N(  c, s );

  return;
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

    CONSUME( fault_rd( stdin ), 1 );

    if( Delta.enable ) {
      if     ( ( Delta.round    < 0 ) || ( Delta.round    > 10 ) ) {
        abort();
      }
      else if( ( Delta.function < 0 ) || ( Delta.function >  3 ) ) {
        abort();
      }
      else if( ( Delta.priority < 0 ) || ( Delta.priority >  1 ) ) {
        abort();
      }
      else if( ( Delta.row      < 0 ) || ( Delta.row      >  3 ) ) {
        abort();
      }
      else if( ( Delta.col      < 0 ) || ( Delta.col      >  3 ) ) {
        abort();
      }
    }

    CONSUME( octetstr_rd( stdin, m, 4 * AES_128_NB ), 4 * AES_128_NB );

    #if CONF( TARGET_R, CID )
    CONSUME( octetstr_rd( stdin, k, 4 * AES_128_NK ), 4 * AES_128_NK );
    #endif

    // 2. execute operation
  
    aes_enc( c, m, k );
 
    #if CONF( DEBUG )
    fprintf( stderr, "Delta.enable   = %d\n", Delta.enable   );
  
    fprintf( stderr, "Delta.round    = %d\n", Delta.round    );
    fprintf( stderr, "Delta.function = %d\n", Delta.function );
    fprintf( stderr, "Delta.priority = %d\n", Delta.priority );
    fprintf( stderr, "Delta.row      = %d\n", Delta.row      );
    fprintf( stderr, "Delta.col      = %d\n", Delta.col      );
  
    fprintf( stderr, "c = " ); octetstr_wr( stderr, c, 4 * AES_128_NB );
    fprintf( stderr, "m = " ); octetstr_wr( stderr, m, 4 * AES_128_NB );
    fprintf( stderr, "k = " ); octetstr_wr( stderr, k, 4 * AES_128_NK );
    #endif

    // 3. produce output
    
                               octetstr_wr( stdout, c, 4 * AES_128_NB ); 
   
    // 4. flush streams

    fflush( stdout );
    fflush( stderr );
  }
  
  return 0;
}

/* Copyright (C) 2017 Daniel Page <dan@phoo.org>
 *
 * Use of this source code is restricted per the CC BY-SA license, a copy of
 * which can be found via http://creativecommons.org (and should be included 
 * as LICENSE.txt within the associated archive or repository).
 */

#include "target.h"

fault_t fault;

int fault_rd( FILE* fd ) {
  char T[ BUFFER ], *Tp = fgets( T, BUFFER, fd ); 

  if( Tp == NULL ) {
                          return 0;
  }

  Tp = strip( Tp );

  if( *Tp == '\x00' ) {
    fault.enable = false; return 1;
  }

  if( 5 != sscanf( Tp, "%d,%d,%d,%d,%d", &fault.round,
                                         &fault.function,
                                         &fault.priority,
                                         &fault.row,
                                         &fault.col ) ) {
    fault.enable = false; return 0;
  }
  else {
    fault.enable =  true; return 1;
  }
}

void aes_enc_step( uint8_t* s, uint8_t* rk, int r, int f ) {
  uint8_t delta;

  if( ( fault.enable                          ) &&
      ( fault.round    == r                   ) &&
      ( fault.function == f                   ) &&
      ( fault.priority == FAULT_PRIORITY_PRE  ) ) { 
    #if CONF( ALLOW_ZERO, CID )
         delta = prng() & 0xFF;
    #else
    do { delta = prng() & 0xFF; } while( delta == 0 );
    #endif 

    s[ ( fault.col * 4 ) + fault.row ] ^= delta;
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

  if( ( fault.enable                          ) &&
      ( fault.round    == r                   ) &&
      ( fault.function == f                   ) &&
      ( fault.priority == FAULT_PRIORITY_POST ) ) { 
    #if CONF( ALLOW_ZERO, CID )
         delta = prng() & 0xFF;
    #else
    do { delta = prng() & 0xFF; } while( delta == 0 );
    #endif 

    s[ ( fault.col * 4 ) + fault.row ] ^= delta;
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

    if( fault.enable ) {
      if     ( ( fault.round    < 0 ) || ( fault.round    > 10 ) ) {
        abort();
      }
      else if( ( fault.function < 0 ) || ( fault.function >  3 ) ) {
        abort();
      }
      else if( ( fault.priority < 0 ) || ( fault.priority >  1 ) ) {
        abort();
      }
      else if( ( fault.row      < 0 ) || ( fault.row      >  3 ) ) {
        abort();
      }
      else if( ( fault.col      < 0 ) || ( fault.col      >  3 ) ) {
        abort();
      }
    }

    CONSUME( octet_rd( stdin, m, 4 * AES_128_NB ), 4 * AES_128_NB );

    #if CONF( TARGET_R, CID )
    CONSUME( octet_rd( stdin, k, 4 * AES_128_NK ), 4 * AES_128_NK );
    #endif

    // 2. execute operation
  
    aes_enc( c, m, k );
 
    #if CONF( DEBUG )
    fprintf( stderr, "fault.enable   = %d\n", fault.enable   );
  
    fprintf( stderr, "fault.round    = %d\n", fault.round    );
    fprintf( stderr, "fault.function = %d\n", fault.function );
    fprintf( stderr, "fault.priority = %d\n", fault.priority );
    fprintf( stderr, "fault.row      = %d\n", fault.row      );
    fprintf( stderr, "fault.col      = %d\n", fault.col      );
  
    fprintf( stderr, "c = " ); octet_wr( stderr, c, 4 * AES_128_NB );
    fprintf( stderr, "m = " ); octet_wr( stderr, m, 4 * AES_128_NB );
    fprintf( stderr, "k = " ); octet_wr( stderr, k, 4 * AES_128_NK );
    #endif

    // 3. produce output
    
                               octet_wr( stdout, c, 4 * AES_128_NB ); 
   
    // 4. flush streams

    fflush( stdout );
    fflush( stderr );
  }
  
  return 0;
}

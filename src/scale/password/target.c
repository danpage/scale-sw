/* Copyright (C) 2017 Daniel Page <dan@phoo.org>
 *
 * Use of this source code is restricted per the CC BY-SA license, a copy of
 * which can be found via http://creativecommons.org (and should be included 
 * as LICENSE.txt within the associated archive or repository).
 */

#include "target.h"

int match( int* r, const char* P, const char* G ) {
  int Lambda;

  int l_P = strlen( P );
  int l_G = strlen( G );

  Lambda = 0;

  if( l_P != l_G ) {
      *r = 0; return Lambda;
  }

  Lambda = 1;

  for( int i = 0; i < l_P; i++, Lambda = Lambda + 1 ) {
    if( P[ i ] != G[ i ] ) {
      *r = 0; return Lambda;
    }
  }

      *r = 1; return Lambda;
}

int main( int argc, char* argv[] ) {
  char G[ CONF( LEN_P_MAX, CID ) + 1 ], P[ CONF( LEN_P_MAX, CID ) + 1 ] = { USER( P_DATA, CID ) };

  #if   CONF( TARGET_D, CID ) &&  CONF( OBFUSCATE )
  unmask( P,    USER( P_SIZE, CID ), USER( P_MASK, CID ) );
  #elif CONF( TARGET_R, CID ) || !CONF( OBFUSCATE )
  memset( P, 0, USER( P_SIZE, CID )                      );
  #endif

  while( true ) {
    int Lambda, r;

    // 1. consume input

    CONSUME( fscanf( stdin, "%" STR( CONF( LEN_P_MAX, CID ) ) "s", G ), 1 );

    #if CONF( TARGET_R, CID )
    CONSUME( fscanf( stdin, "%" STR( CONF( LEN_P_MAX, CID ) ) "s", P ), 1 );
    #endif

    // 2. execute operation

    Lambda = match( &r, G, P );

    #if CONF( DEBUG )
    fprintf( stderr, "G = %s\n", G      );
    fprintf( stderr, "P = %s\n", P      );
    #endif

    // 3. produce output

    fprintf( stdout,     "%d\n", Lambda );
    fprintf( stdout,     "%d\n", r      );

    // 5. flush streams

    fflush( stdout );
    fflush( stderr );
  }

  return 0;
}

/* Copyright (C) 2017 Daniel Page <dan@phoo.org>
 *
 * Use of this source code is restricted per the CC BY-SA license, a copy of
 * which can be found via http://creativecommons.org (and should be included 
 * as LICENSE.txt within the associated archive or repository).
 */

#include "util.h"

int xtoi( char x ) {
  if      ( x >= '0' && x <= '9' ) {
    return ( ( int )( x ) - ( int )( '0' )      );
  }
  else if ( x >= 'a' && x <= 'f' ) {
    return ( ( int )( x ) - ( int )( 'a' ) + 10 );
  }
  else if ( x >= 'A' && x <= 'F' ) {
    return ( ( int )( x ) - ( int )( 'A' ) + 10 );
  }

  return -1;
}

void unmask( uint8_t* x, int n, uint8_t m ) {
  for( int i = 0; i < n; i++ ) {
    x[ i ] = x[ i ] ^ m;
  }
}

bool memeqz( const uint8_t* x, int n ) {
  for( int i = 0; i < n; i++ ) {
    if( x[ i ] != 0 ) {
      return false;
    }
  }

  return true;
}

FILE* dev = NULL;

uint8_t prng() {
  if( dev == NULL ) {
    dev = fopen( "/dev/urandom", "rb" );
  }

  return ( uint8_t )( fgetc( dev ) );
}

uint16_t prng_16() {
  return ( ( uint16_t )( prng() ) <<  0 ) |
         ( ( uint16_t )( prng() ) <<  8 ) ;
}

uint32_t prng_32() {
  return ( ( uint32_t )( prng() ) <<  0 ) |
         ( ( uint32_t )( prng() ) <<  8 ) |
         ( ( uint32_t )( prng() ) << 16 ) |
         ( ( uint32_t )( prng() ) << 24 ) ;
}

char* strip( char* x ) {
  int n = strlen( x );

  while( isspace( *( x + n - 1 ) ) ) {
    *( x + n - 1 ) = '\x00'; n--;
  }
  while( isspace( *( x         ) ) ) {
                             x++;
  }

  return x;
}

void octet_wr( FILE* fd, const uint8_t* x, int n_x ) {
  for( int i = 0; i < n_x; i++ ) {
    fprintf( fd, "%02X", x[ i ] );
  }

  fprintf( fd, "\n" );
}

int  octet_rd( FILE* fd,       uint8_t* r, int n_r ) {
  char T[ BUFFER ], *Tp = fgets( T, BUFFER, fd );

  if( Tp == NULL ) {
    return 0;
  }

  Tp = strip( Tp );

  int i;

  for( i = 0; *Tp != '\x00'; i++ ) {
    uint8_t t = 0;

    if( *Tp != '\x00' ) {
      if( !isxdigit( *Tp ) ) {
	break;
      }

      t |= ( uint8_t )( xtoi( *Tp++ ) ) << 4;
    }

    if( *Tp != '\x00' ) {
      if( !isxdigit( *Tp ) ) {
	break;
      }

      t |= ( uint8_t )( xtoi( *Tp++ ) ) << 0;
    }

    if( i < n_r ) {
      r[ i ] = t;
    }
  }

  return i;
}

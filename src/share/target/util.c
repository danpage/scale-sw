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

char itox( int x ) {
  if     ( x >=  0 && x <=  9 ) {
    return ( char )( ( int )( '0' ) + x       );
  }
  else if( x >= 10 && x <= 15 ) {
    return ( char )( ( int )( 'A' ) + x - 10  );
  }

  return '?';
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

int  octetstr_rd( FILE* fd,       uint8_t* r, int n_r ) {
  int n  = ( ( xtoi( fgetc( fd ) ) & 0xF ) << 4 );
      n |= ( ( xtoi( fgetc( fd ) ) & 0xF ) << 0 );

  if( n > n_r ) {
    return -1;
  }

  if( fgetc( fd ) != ':' ) {
    return -1;
  }

  uint8_t t;

  for( int i = 0; i < n; i++ ) {
    r[ i ] = 0;

    if( ( t = fgetc( fd ) ) != '\x0A' ) {
      if( !isxdigit( t ) ) {
	break;
      }

      r[ i ] |= ( uint8_t )( xtoi( t ) & 0xF ) << 4;
    }
    if( ( t = fgetc( fd ) ) != '\x0A' ) {
      if( !isxdigit( t ) ) {
	break;
      }

      r[ i ] |= ( uint8_t )( xtoi( t ) & 0xF ) << 0;
    }
  }

  do {
    t = fgetc( fd );
  }
  while( t != '\x0A' );

  return n;
}

void octetstr_wr( FILE* fd, const uint8_t* x, int n_x ) {
             fputc( itox( ( n_x    >> 4 ) & 0xF ), fd );
             fputc( itox( ( n_x    >> 0 ) & 0xF ), fd );

             fputc( '\x3A', fd );

  for( int i = 0; i < n_x; i++ ) {
             fputc( itox( ( x[ i ] >> 4 ) & 0xF ), fd );
             fputc( itox( ( x[ i ] >> 0 ) & 0xF ), fd );
  }

             fputc( '\x0A', fd );
}

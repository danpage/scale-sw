/* Copyright (C) 2017 Daniel Page <dan@phoo.org>
 *
 * Use of this source code is restricted per the CC BY-SA license, a copy of
 * which can be found via http://creativecommons.org (and should be included 
 * as LICENSE.txt within the associated archive or repository).
 */

#include "wrapper.h"

void aes_enc( uint8_t* c, const uint8_t* m, const uint8_t* k ) {
  i8051_rst();
	
    i8051_core->iram[ 0x30 + 0x00     ] = 0x00;

  for( int i = 0; i < ( 4 * AES_128_NB ); i++ ) {
    i8051_core->iram[ 0x30 + 0x01 + i ] = m[ i ];
  }
  for( int i = 0; i < ( 4 * AES_128_NK ); i++ ) {
    i8051_core->iram[ 0x30 + 0x11 + i ] = k[ i ];
  }
  
  i8051_core_whole();
   
  for( int i = 0; i < ( 4 * AES_128_NB ); i++ ) {
    c[ i ] = i8051_core->iram[ 0x30 + 0x01 + i ];
  }

  return;
}

void aes_dec( uint8_t* m, const uint8_t* c, const uint8_t* k ) {
  i8051_rst();
	
    i8051_core->iram[ 0x30 + 0x00     ] = 0x01;
  for( int i = 0; i < ( 4 * AES_128_NB ); i++ ) {
    i8051_core->iram[ 0x30 + 0x01 + i ] = c[ i ];
  }
  for( int i = 0; i < ( 4 * AES_128_NK ); i++ ) {
    i8051_core->iram[ 0x30 + 0x11 + i ] = k[ i ];
  }
  
  i8051_core_whole();
   
  for( int i = 0; i < ( 4 * AES_128_NB ); i++ ) {
    m[ i ] = i8051_core->iram[ 0x30 + 0x01 + i ];
  }

  return;
}

/*
IEEE 169-2007, Section 5.2 
arithmetic in F_2[x] / x^128 + x^7 + x^2 + x + 1
*/

uint8_t alpha[] = { 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

void gf128_add( uint8_t* r, const uint8_t* x, const uint8_t* y ) {
  for( int i = 0; i < 16; i++ ) {
    r[ i ] = x[ i ] ^ y[ i ];
  }

  return;
}

void gf128_sub( uint8_t* r, const uint8_t* x, const uint8_t* y ) {
  for( int i = 0; i < 16; i++ ) {
    r[ i ] = x[ i ] ^ y[ i ];
  }

  return;
}

void gf128_mul( uint8_t* r, const uint8_t* x, const uint8_t* y ) {
  uint8_t t[ 16 ];

  memset( t, 0, 16 * SIZEOF( uint8_t ) );

  for( int i = 16 - 1; i >= 0; i-- ) {
    for( int j = BITSOF( uint8_t ) - 1; j >= 0; j-- ) {
      uint8_t ci = 0, co = 0;

      for( int k = 0; k < 16; k++ ) {
        co = t[ k ] >> 7; t[ k ] = ( t[ k ] << 1 ) | ci; ci = co;
      }

      if( co ) {
        t[ 0 ] ^= 0x87; 
      }
    
      if( ( y[ i ] >> j ) & 1 ) {
        for( int k = 0; k < 16; k++ ) {
          t[ k ] ^= x[ k ];
        }
      }
    }
  }

  memcpy( r, t, 16 * SIZEOF( uint8_t ) );

  return;
}

void gf128_exp( uint8_t* r, const uint8_t* x, int y ) {
  uint8_t t[ 16 ];

  memset( t, 0, 16 * SIZEOF( uint8_t ) ); t[ 0 ] = 0x01; 

  for( int i = BITSOF( int ) - 1; i >= 0; i-- ) {
    gf128_mul( t, t, t );

    if( ( y >> i ) & 1 ) {
      gf128_mul( t, t, x );
    }
  }

  memcpy( r, t, 16 * SIZEOF( uint8_t ) );

  return;
}

/*
IEEE 169-2007, Section 5.3.1: 
XTS-AES-blockEnc
*/

void xts_aes_enc( uint8_t* c, const uint8_t* m, const uint8_t* k, const uint8_t* i, int j ) {
  const uint8_t* k1 = k + ( 0 * 4 * AES_128_NK );
  const uint8_t* k2 = k + ( 1 * 4 * AES_128_NK );
  
        uint8_t  t0[ 4 * AES_128_NB ];
        uint8_t  t1[ 4 * AES_128_NB ];
   
  gf128_exp( t1, alpha,  j ); //                            \alpha^j
    aes_enc( t0,     i, k2 ); //      Enc( k_2, i )
  gf128_mul( t0,    t0, t1 ); // t  = Enc( k_2, i ) \otimes \alpha^j
  
  gf128_add( t1,     m, t0 ); // pp =  p \oplus t
    aes_enc( t1,    t1, k1 ); // cc = Enc( k_1, pp )
  gf128_add(  c,    t1, t0 ); // c  = cc \oplus t

  return;
}

/*
IEEE 169-2007, Section 5.4.1: 
XTS-AES-blockDec
*/

void xts_aes_dec( uint8_t* m, const uint8_t* c, const uint8_t* k, const uint8_t* i, int j ) {
  const uint8_t* k1 = k + ( 0 * 4 * AES_128_NK );
  const uint8_t* k2 = k + ( 1 * 4 * AES_128_NK );
  
        uint8_t  t0[ 4 * AES_128_NB ];
        uint8_t  t1[ 4 * AES_128_NB ];

  gf128_exp( t1, alpha,  j ); //                            \alpha^j
    aes_enc( t0,     i, k2 ); //      Enc( k_2, i )
  gf128_mul( t0,    t0, t1 ); // t  = Enc( k_2, i ) \otimes \alpha^j
  
  gf128_add( t1,     c, t0 ); // cc =  c \oplus t
    aes_dec( t1,    t1, k1 ); // pp = Dec( k_1, cc )
  gf128_add(  m,    t1, t0 ); // p  = pp \oplus t

  return;
}

void trace_rd( FILE* fd ) {
  return;
}

void trace_wr( FILE* fd ) {
  fprintf( fd, "%d", i8051_trace_used );

  for( int i = 0; i < i8051_trace_used; i++ ) {
    int x = i8051_trace[ i ];

    #if ( ( CONF( NOISE_MAX, CID ) - CONF( NOISE_MIN, CID ) ) > 0 )
    x += CONF( NOISE_MIN, CID ) + ( prng_32() % ( CONF( NOISE_MAX, CID ) - CONF( NOISE_MIN, CID ) ) );
    #endif

    if     ( x <   0 ) {
      x =   0;
    }
    else if( x > 255 ) {
      x = 255;
    }

    fprintf( fd, ",%d", x );
  }

  fprintf( fd, "\n" );

  return;
}

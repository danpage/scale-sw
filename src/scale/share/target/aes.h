/* Copyright (C) 2017 Daniel Page <dan@phoo.org>
 *
 * Use of this source code is restricted per the CC BY-SA license, a copy of
 * which can be found via http://creativecommons.org (and should be included 
 * as LICENSE.txt within the associated archive or repository).
 */

#ifndef __AES_H
#define __AES_H

#include "util.h"

// Nb = columns in state, Nk = columns in key, Nr = number of rounds

#define AES_128_NB (  4 )
#define AES_192_NB (  4 )
#define AES_256_NB (  4 )

#define AES_128_NK (  4 )
#define AES_192_NK (  6 )
#define AES_256_NK (  8 )

#define AES_128_NR ( 10 )
#define AES_192_NR ( 12 )
#define AES_256_NR ( 14 )

#define AES_128_SIZEOF_BLOCK ( 4 * AES_128_NB )
#define AES_192_SIZEOF_BLOCK ( 4 * AES_192_NB )
#define AES_256_SIZEOF_BLOCK ( 4 * AES_256_NB )
#define AES_128_SIZEOF_KEY   ( 4 * AES_128_NK )
#define AES_192_SIZEOF_KEY   ( 4 * AES_192_NK )
#define AES_256_SIZEOF_KEY   ( 4 * AES_256_NK )

#define AES_ENC_KEY_STEP(a,b,c,d) { \
  s[ a ] = s[ a ] ^ k[ a ];         \
  s[ b ] = s[ b ] ^ k[ b ];         \
  s[ c ] = s[ c ] ^ k[ c ];         \
  s[ d ] = s[ d ] ^ k[ d ];         \
}
#define AES_ENC_SUB_STEP(a,b,c,d) { \
  s[ a ] = AES_ENC_SBOX[ s[ a ] ];  \
  s[ b ] = AES_ENC_SBOX[ s[ b ] ];  \
  s[ c ] = AES_ENC_SBOX[ s[ c ] ];  \
  s[ d ] = AES_ENC_SBOX[ s[ d ] ];  \
}
#define AES_ENC_ROW_STEP(a,b,c,d,e,f,g,h) { \
  gf28_t a1 = s[ a ], b1 = s[ b ];          \
  gf28_t c1 = s[ c ], d1 = s[ d ];          \
                                            \
  s[ e ] = a1; s[ f ] = b1;                 \
  s[ g ] = c1; s[ h ] = d1;                 \
}
#define AES_ENC_MIX_STEP(a,b,c,d) {         \
  gf28_t a1 = s[ a ], a2 = GF28_MULX[ a1 ]; \
  gf28_t b1 = s[ b ], b2 = GF28_MULX[ b1 ]; \
  gf28_t c1 = s[ c ], c2 = GF28_MULX[ c1 ]; \
  gf28_t d1 = s[ d ], d2 = GF28_MULX[ d1 ]; \
                                            \
  gf28_t a3 = a1 ^ a2;                      \
  gf28_t b3 = b1 ^ b2;                      \
  gf28_t c3 = c1 ^ c2;                      \
  gf28_t d3 = d1 ^ d2;                      \
                                            \
  s[ a ] = a2 ^ b3 ^ c1 ^ d1;               \
  s[ b ] = a1 ^ b2 ^ c3 ^ d1;               \
  s[ c ] = a1 ^ b1 ^ c2 ^ d3;               \
  s[ d ] = a3 ^ b1 ^ c1 ^ d2;               \
}

#define AES_DEC_KEY_STEP(a,b,c,d) { \
  s[ a ] = s[ a ] ^ k[ a ];         \
  s[ b ] = s[ b ] ^ k[ b ];         \
  s[ c ] = s[ c ] ^ k[ c ];         \
  s[ d ] = s[ d ] ^ k[ d ];         \
}
#define AES_DEC_SUB_STEP(a,b,c,d) { \
  s[ a ] = AES_DEC_SBOX[ s[ a ] ];  \
  s[ b ] = AES_DEC_SBOX[ s[ b ] ];  \
  s[ c ] = AES_DEC_SBOX[ s[ c ] ];  \
  s[ d ] = AES_DEC_SBOX[ s[ d ] ];  \
}
#define AES_DEC_ROW_STEP(a,b,c,d,e,f,g,h) { \
  gf28_t a1 = s[ a ], b1 = s[ b ];          \
  gf28_t c1 = s[ c ], d1 = s[ d ];          \
                                            \
  s[ e ] = a1; s[ f ] = b1;                 \
  s[ g ] = c1; s[ h ] = d1;                 \
}
#define AES_DEC_MIX_STEP(a,b,c,d) {         \
  gf28_t a1 = s[ a ], a2 = GF28_MULX[ a1 ]; \
  gf28_t b1 = s[ b ], b2 = GF28_MULX[ b1 ]; \
  gf28_t c1 = s[ c ], c2 = GF28_MULX[ c1 ]; \
  gf28_t d1 = s[ d ], d2 = GF28_MULX[ d1 ]; \
                                            \
  gf28_t              a4 = GF28_MULX[ a2 ]; \
  gf28_t              b4 = GF28_MULX[ b2 ]; \
  gf28_t              c4 = GF28_MULX[ c2 ]; \
  gf28_t              d4 = GF28_MULX[ d2 ]; \
                                            \
  gf28_t              a8 = GF28_MULX[ a4 ]; \
  gf28_t              b8 = GF28_MULX[ b4 ]; \
  gf28_t              c8 = GF28_MULX[ c4 ]; \
  gf28_t              d8 = GF28_MULX[ d4 ]; \
                                            \
  gf28_t aA = a2 ^ a8, aC = a4 ^ a8;        \
  gf28_t bA = b2 ^ b8, bC = b4 ^ b8;        \
  gf28_t cA = c2 ^ c8, cC = c4 ^ c8;        \
  gf28_t dA = d2 ^ d8, dC = d4 ^ d8;        \
                                            \
  s[ a ] = ( aC ^ a2 ) ^ ( bA ^ b1 ) ^      \
           ( cC ^ c1 ) ^ ( d8 ^ d1 ) ;      \
  s[ b ] = ( a8 ^ a1 ) ^ ( bC ^ b2 ) ^      \
           ( cA ^ c1 ) ^ ( dC ^ d1 ) ;      \
  s[ c ] = ( aC ^ a1 ) ^ ( b8 ^ b1 ) ^      \
           ( cC ^ c2 ) ^ ( dA ^ d1 ) ;      \
  s[ d ] = ( aA ^ a1 ) ^ ( bC ^ b1 ) ^      \
           ( c8 ^ c1 ) ^ ( dC ^ d2 ) ;      \
}

typedef uint8_t gf28_t;

extern const gf28_t GF28_MULX[]; 
extern const gf28_t GF28_DIVX[]; 

extern const gf28_t AES_ENC_SBOX[];
extern const gf28_t AES_DEC_SBOX[];

extern void   aes_enc_key( gf28_t* s, const gf28_t* k );
extern void   aes_enc_sub( gf28_t* s );
extern void   aes_enc_row( gf28_t* s );
extern void   aes_enc_mix( gf28_t* s );

extern void   aes_dec_key( gf28_t* s, const gf28_t* k );
extern void   aes_dec_sub( gf28_t* s );
extern void   aes_dec_row( gf28_t* s );
extern void   aes_dec_mix( gf28_t* s );

extern gf28_t aes_enc_keyexp_step( gf28_t* r, const gf28_t* k, gf28_t rc );
extern gf28_t aes_dec_keyexp_step( gf28_t* r, const gf28_t* k, gf28_t rc );

#endif


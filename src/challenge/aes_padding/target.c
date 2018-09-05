/* Copyright (C) 2017 Daniel Page <dan@phoo.org>
 *
 * Use of this source code is restricted per the CC BY-SA license, a copy of
 * which can be found via http://creativecommons.org (and should be included 
 * as LICENSE.txt within the associated archive or repository).
 */

#include "target.h"

int dec( uint8_t* m, 
         uint8_t* iv, 
         uint8_t* c,     int l, 
         uint8_t* k_enc, int n_k_enc, 
         uint8_t* k_mac, int n_k_mac ) {

  // decrypt ciphertext

  AES_KEY rk;

  AES_set_decrypt_key( k_enc, n_k_enc * 8, &rk );
  AES_cbc_encrypt( c, m, l * 4 * AES_128_NB, &rk, iv, AES_DECRYPT );

  // check validity of padding

  uint8_t *t = m + l * 4 * AES_128_NB - 1, p = *t--;

  for( int i = 0; i < p; i++, t-- ) {
    if( p != *t ) {
      return RESULT_CODE_1;
    }
  }

  // abort if unpadded message is too short to represent a MAC tag

  l = t + 1 - m;

  if( l < SHA_DIGEST_LENGTH ) {
    return RESULT_CODE_2;
  }

  // check validity of MAC tag via HMAC-SHA1

  uint8_t d[ SHA_DIGEST_LENGTH ];

  HMAC( EVP_sha1(), k_mac, n_k_mac, m, l - SHA_DIGEST_LENGTH, d, NULL );

  if( memcmp( d, m + l - SHA_DIGEST_LENGTH, SHA_DIGEST_LENGTH ) ) {
    return RESULT_CODE_2;
  }

  // passed padding and MAC checks: message is processed successfully

  return RESULT_CODE_0;
}

int main( int argc, char* argv[] ) {
  uint8_t *iv = NULL, *m = NULL, *c = NULL, k_enc[] = { USER( K_ENC_DATA, CID ) }, k_mac[] = { USER( K_MAC_DATA, CID ) };

  #if   CONF( TARGET_D, CID ) &&  CONF( OBFUSCATE )
  unmask( k_enc,    USER( K_ENC_SIZE, CID ), USER( K_ENC_MASK, CID ) );
  unmask( k_mac,    USER( K_MAC_SIZE, CID ), USER( K_MAC_MASK, CID ) );
  #elif CONF( TARGET_R, CID ) || !CONF( OBFUSCATE )
  memset( k_enc, 0, USER( K_ENC_SIZE, CID ), USER( K_ENC_MASK, CID ) );
  memset( k_mac, 0, USER( K_MAC_SIZE, CID ), USER( K_MAC_MASK, CID ) );
  #endif

  OpenSSL_add_all_algorithms();

  while( true ) {
    // 1. consume input

    int l;

    CONSUME( fscanf( stdin, "%d\n", &l ), 1 );

    if( ( l <= 0 ) || ( l >= 256 ) ) {
      abort();
    }
    else {
      iv = ( uint8_t* )( realloc( iv, ( 1 * 4 * AES_128_NB ) * SIZEOF( uint8_t ) ) );
      m  = ( uint8_t* )( realloc(  m, ( l * 4 * AES_128_NB ) * SIZEOF( uint8_t ) ) );
      c  = ( uint8_t* )( realloc(  c, ( l * 4 * AES_128_NB ) * SIZEOF( uint8_t ) ) );
    }

    CONSUME( octetstr_rd( stdin,    iv, 1 * 4 * AES_128_NB ), 1 * 4 * AES_128_NB );
    CONSUME( octetstr_rd( stdin,     c, l * 4 * AES_128_NB ), l * 4 * AES_128_NB );

    #if CONF( TARGET_R, CID )
    CONSUME( octetstr_rd( stdin, k_enc, 1 * 4 * AES_128_NK ), 1 * 4 * AES_128_NK );
    CONSUME( octetstr_rd( stdin, k_mac, 1 * 4 * AES_128_NK ), 1 * 4 * AES_128_NK );
    #endif

    // 2. execute operation

    int lambda = dec( m, iv, c, l, k_enc, 4 * AES_128_NK, k_mac, 4 * AES_128_NK );

    #if CONF( DEBUG )
    fprintf( stderr, "iv    = " ); octetstr_wr( stderr,    iv, 1 * 4 * AES_128_NB );
    fprintf( stderr, "c     = " ); octetstr_wr( stderr,     c, l * 4 * AES_128_NB );
    fprintf( stderr, "m     = " ); octetstr_wr( stderr,     m, l * 4 * AES_128_NB );
    fprintf( stderr, "k_enc = " ); octetstr_wr( stderr, k_enc, 1 * 4 * AES_128_NK );
    fprintf( stderr, "k_mac = " ); octetstr_wr( stderr, k_mac, 1 * 4 * AES_128_NK );
    #endif

    // 3. produce output

    fprintf( stdout, "%d\n", lambda ); 

    // 4. flush streams

    fflush( stdout );
    fflush( stderr );
  }
  
  return 0;
}

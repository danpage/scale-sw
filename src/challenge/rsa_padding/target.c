/* Copyright (C) 2017 Daniel Page <dan@phoo.org>
 *
 * Use of this source code is restricted per the CC BY-SA license, a copy of
 * which can be found via http://creativecommons.org (and should be included 
 * as LICENSE.txt within the associated archive or repository).
 */

#include "target.h"

jmp_buf env;

/*
RFC 3447, Sec. 4.1 + 4.2 
convert integer to/from (big-endian) octet string
*/

void i2osp( uint8_t* r, int n_r, const mpz_t x ) {
  int l = CEIL( mpz_sizeinbase( x, 2 ), 8 );

  memset( r, 0, n_r );

  mpz_export( r + ( n_r - l ), NULL, +1, SIZEOF( uint8_t ), -1, 0, x );

  return;
}

void os2ip( mpz_t r, const uint8_t* x, int n_x ) {
  mpz_import( r,                n_x, +1, SIZEOF( uint8_t ), -1, 0, x );

  return;
}

/*
RFC 3447, Sec. B.2.1
mask generation function: produce n_r octets of output into r, based
on a seed x of n_x octets
*/

void mgf1( uint8_t* r, int n_r, const uint8_t* x, int n_x ) {
  int n_d = HASH_LEN, n_c = SIZEOF( uint32_t ) / SIZEOF( uint8_t ), n;

  if     ( ( n_r % n_d ) == 0 ) {
    n = ( n_r / n_d ) + 0;
  }
  else if( ( n_r % n_d ) != 0 ) {
    n = ( n_r / n_d ) + 1;
  }

  uint8_t d[ n * n_d ], c[ n_c ];

  for( int i = 0; i < n; i++ ) {
    c[ 0 ] = ( i >> 24 ) & 0xFF;
    c[ 1 ] = ( i >> 16 ) & 0xFF;
    c[ 2 ] = ( i >>  8 ) & 0xFF;
    c[ 3 ] = ( i >>  0 ) & 0xFF;

    HASH_CTX ctx;

    HASH_INIT( &ctx );
    HASH_STEP( &ctx, x, n_x );
    HASH_STEP( &ctx, c, n_c );
    HASH_FINI( d + ( i * n_d ), &ctx );
  }

  memcpy( r, d, n_r );

  return;
}

/*
RFC 3447, Sec. 7.1.1

- RSAEP              : take an (integer) plaintext  m, encrypt it to form 
                       an (integer) ciphertext c under public  key (N,e)
- EME-OAEP encoding  : take a plaintext m of n_m octets, and encode it to
                       form the result r of n_r octets (using label l of
                       n_l octets)
- RSAES-OAEP-Encrypt : apply EME-OAEP encoding + RSAEP (plus conversions)
*/

void rsaep( mpz_t c, const mpz_t N, const mpz_t e, const mpz_t m ) {
  if( ( mpz_sgn( m ) < 0 ) || ( mpz_cmp( m, N ) >= 0 ) ) {
    longjmp( env, RESULT_CODE_3 );
  }

  mpz_powm( c, m, e, N );

  return;
}

int eme_oaep_encode( uint8_t* r, int n_r, const uint8_t* m, int n_m, const uint8_t* l, int n_l ) {
  int n_d = HASH_LEN;

  uint8_t  L[       n_d     ]; 
  uint8_t DB[ n_r - n_d - 1 ]; uint8_t* DBp = DB;
  uint8_t DM[ n_r - n_d - 1 ]; uint8_t* DMp = DM;
  uint8_t SB[       n_d     ]; uint8_t* SBp = SB;
  uint8_t SM[       n_d     ]; uint8_t* SMp = SM;

  HASH( l, n_l, L );

  memcpy( DBp, L,             n_d             ); 
  DBp +=        (             n_d             );  
  memset( DBp, 0, n_r - ( 2 * n_d ) - 2 - n_m );
  DBp +=        ( n_r - ( 2 * n_d ) - 2 - n_m );  
  memset( DBp, 1,                     1       );
  DBp +=        (                     1       );
  memcpy( DBp, m,                         n_m );
  DBp +=        (                         n_m );  

  for( int i = 0; i < n_d; i++ ) {
    SBp[ i ] = ( prng() & 0xFF );
  }

  mgf1( DM, n_r - n_d - 1, SB,       n_d     );

  for( int i = 0; i < ( n_r - n_d - 1 ); i++ ) {
    DB[ i ] ^= DM[ i ];
  }

  mgf1( SM,       n_d,     DB, n_r - n_d - 1 );

  for( int i = 0; i < (       n_d     ); i++ ) {
    SB[ i ] ^= SM[ i ];
  }

  memset( r,            0,             1 ); // r = 00
  memcpy( r + 1,       SB,       n_d     ); // r = 00 || masked seed
  memcpy( r + 1 + n_d, DB, n_r - n_d - 1 ); // r = 00 || masked seed || masked DB

  return n_r;
}

int rsaes_oaep_encrypt(       uint8_t* c,          
                        const   mpz_t  N, 
                        const   mpz_t  e,          
                        const uint8_t* m, int n_m,  
                        const uint8_t* l, int n_l ) {

  int n_d = HASH_LEN, k = CEIL( mpz_sizeinbase( N, 2 ), 8 );

  if( n_m > ( k - ( 2 * n_d ) - 2 ) ) {
    longjmp( env, RESULT_CODE_5 );
  }

  uint8_t t[ k ]; mpz_t t_0, t_1;

  // apply EME-OAEP encoding: encode m into t

  int n_c = eme_oaep_encode( t, k, m, n_m, l, n_l );

  // apply RSAEP

  mpz_init( t_0 );
  mpz_init( t_1 );

  os2ip( t_0, t, k   );    // t_0 = integer      version of t
  rsaep( t_1, N, e, t_0 ); // t_1 = encryption of t_0
  i2osp( c, n_c, t_1 );    // c   = octet string version of t_1

  mpz_clear( t_0 );
  mpz_clear( t_1 );

  return k;
}

/*
RFC 3447, Sec. 7.1.2

- RSAEP              : take an (integer) plaintext  m, encrypt it to form 
                       an (integer) ciphertext c under public  key (N,e)
- EME-OAEP encoding  : take a plaintext m of n_m octets, and encode it to
                       form the result r of n_r octets (using label l of
                       n_l octets)
- RSAES-OAEP-Encrypt : apply RSADP + EME-OAEP decoding (plus conversions)
*/

void rsadp( mpz_t m, const mpz_t N, const mpz_t d, const mpz_t c ) {
  if( ( mpz_sgn( c ) < 0 ) || ( mpz_cmp( c, N ) >= 0 ) ) {
    longjmp( env, RESULT_CODE_4 );
  }

  mpz_powm( m, c, d, N );

  return;
}

int eme_oaep_decode( uint8_t* r, const uint8_t* m, int n_m, const uint8_t* l, int n_l ) {
  int n_d = HASH_LEN, n_r;
 
  uint8_t  L[       n_d     ]; 
  uint8_t DB[ n_m - n_d - 1 ]; uint8_t* DBp = DB;
  uint8_t DM[ n_m - n_d - 1 ]; uint8_t* DMp = DM;
  uint8_t SB[       n_d     ]; uint8_t* SBp = SB;
  uint8_t SM[       n_d     ]; uint8_t* SMp = SM;

  if( m[ 0 ] != 0 ) {
    longjmp( env, RESULT_CODE_1 );
  }

  memcpy( SB,       m + 1,       n_d     );
  memcpy( DB, n_d + m + 1, n_m - n_d - 1 );

  mgf1( SM,       n_d,     DB, n_m - n_d - 1 );

  for( int i = 0; i < (       n_d     ); i++ ) {
    SB[ i ] ^= SM[ i ];
  }

  mgf1( DM, n_m - n_d - 1, SB,       n_d     );

  for( int i = 0; i < ( n_m - n_d - 1 ); i++ ) {
    DB[ i ] ^= DM[ i ];
  }

  HASH( l, n_l, L );

  for( int i = 0; i < n_d; i++ ) {
    if( DBp[ i ] != L[ i ] ) {
      longjmp( env, RESULT_CODE_2 );
    }
  }

  DBp += n_d;

  while( *DBp == 0 ) {
    DBp += 1;  
  }

  if   ( *DBp == 1 ) {
    DBp += 1;  
  }
  else {
    longjmp( env, RESULT_CODE_2 );
  }

  memcpy( r, DBp, n_r = n_m - n_d - 1 - ( DBp - DB ) );

  return n_r;
}

int rsaes_oaep_decrypt(       uint8_t* m,          
                        const   mpz_t  N,          
                        const   mpz_t  d,          
                        const uint8_t* c, int n_c,  
                        const uint8_t* l, int n_l ) {

  int n_d = HASH_LEN, k = CEIL( mpz_sizeinbase( N, 2 ), 8 );

  if( n_c != k ) {
    longjmp( env, RESULT_CODE_6 );
  }

  if( k < ( ( 2 * n_d ) + 2 ) ) {
    longjmp( env, RESULT_CODE_7 );
  }

  uint8_t t[ k ]; mpz_t t_0, t_1;

  // apply RSADP

  mpz_init( t_0 );
  mpz_init( t_1 );

  os2ip( t_1, c, n_c );    // t_1 = integer      version of c
  rsadp( t_0, N, d, t_1 ); // t_0 = decryption of t_1

  i2osp( t, k, t_0   );    // t   = octet string version of t_0

  mpz_clear( t_0 );
  mpz_clear( t_1 );

  // apply EME-OAEP decoding: decode t into m

  int n_m = eme_oaep_decode( m, t, k, l, n_l );

  return n_m;
}

int main( int argc, char* argv[] ) {
  uint8_t __N[] = { USER( N_DATA, CID ) };
  uint8_t __e[] = { USER( E_DATA, CID ) };
  uint8_t __d[] = { USER( D_DATA, CID ) };

  mpz_t N, e, d;

  #if   CONF( TARGET_D, CID ) &&  CONF( OBFUSCATE )
  unmask( __N, USER( N_SIZE, CID ), USER( N_MASK, CID ) ); 
  unmask( __e, USER( E_SIZE, CID ), USER( E_MASK, CID ) ); 
  unmask( __d, USER( D_SIZE, CID ), USER( D_MASK, CID ) );
  #endif

  mpz_inits( N, e, d, NULL );

  mpz_import( N, USER( N_SIZE, CID ), -1, SIZEOF( uint8_t ), -1, 0, __N );
  mpz_import( e, USER( E_SIZE, CID ), -1, SIZEOF( uint8_t ), -1, 0, __e );
  mpz_import( d, USER( D_SIZE, CID ), -1, SIZEOF( uint8_t ), -1, 0, __d );
  
  int k = CEIL( mpz_sizeinbase( N, 2 ), 8 );

  uint8_t l[ k ]; int n_l;
  uint8_t m[ k ]; int n_m;
  uint8_t c[ k ]; int n_c;

  OpenSSL_add_all_algorithms();

  while( true ) {
    // 1. consume input
  
    CONSUME( n_l = octet_rd( stdin, l, k ) );
    CONSUME( n_c = octet_rd( stdin, c, k ) );

    // 2. execute operation

    int lambda;

    if( !( lambda = setjmp( env ) ) ) {
      n_m = rsaes_oaep_decrypt( m, N, d, c, n_c, l, n_l ); lambda = RESULT_CODE_0;
    }

    #if CONF( DEBUG )
    fprintf( stderr, "N = " ); gmp_fprintf( stderr, "%ZX\n", N      );
    fprintf( stderr, "e = " ); gmp_fprintf( stderr, "%ZX\n", e      );
    fprintf( stderr, "d = " ); gmp_fprintf( stderr, "%ZX\n", d      );

    fprintf( stderr, "k = " ); gmp_fprintf( stderr,  "%d\n", k      );

    fprintf( stderr, "l = " );    octet_wr( stderr, l, n_l );
    fprintf( stderr, "m = " );    octet_wr( stderr, m, n_m );
    fprintf( stderr, "c = " );    octet_wr( stderr, c, n_c );
    #endif

    // 3. produce output

                               gmp_fprintf( stdout,  "%d\n", lambda );

    // 4. flush streams

    fflush( stdout );
    fflush( stderr );
  }

  mpz_clears( N, e, d, NULL );

  return 0;
}

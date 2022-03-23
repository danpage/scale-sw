/* Copyright (C) 2017 Daniel Page <dan@phoo.org>
 *
 * Use of this source code is restricted per the CC BY-SA license, a copy of
 * which can be found via http://creativecommons.org (and should be included 
 * as LICENSE.txt within the associated archive or repository).
 */

#include "target.h"

void mul( EC_GROUP* G, EC_POINT* R, const EC_POINT* P, const BIGNUM* k, BN_CTX* ctx ) {
  EC_POINT* T = EC_POINT_new( G );

  EC_POINT_set_to_infinity( G, T );

  for( int i = BN_num_bits( k ) - 1; i >= 0; i-- ) {
    EC_POINT_dbl( G, T, T, ctx );

    if( BN_is_bit_set( k, i ) ) {
      EC_POINT_add( G, T, T, P, ctx );
    }
  }

  EC_POINT_copy( R, T ); EC_POINT_free( T );
}


int main( int argc, char* argv[] ) {
  uint8_t __k[] = { USER( K_DATA, CID ) };

  #if   CONF( CURVE, CID ) == 0
  EC_GROUP* G   = EC_GROUP_new_by_curve_name( EC_curve_nist2nid( "P-192" ) );
  #elif CONF( CURVE, CID ) == 1
  EC_GROUP* G   = EC_GROUP_new_by_curve_name( EC_curve_nist2nid( "P-224" ) );
  #elif CONF( CURVE, CID ) == 2
  EC_GROUP* G   = EC_GROUP_new_by_curve_name( EC_curve_nist2nid( "P-256" ) );
  #elif CONF( CURVE, CID ) == 3
  EC_GROUP* G   = EC_GROUP_new_by_curve_name( EC_curve_nist2nid( "P-384" ) );
  #elif CONF( CURVE, CID ) == 4
  EC_GROUP* G   = EC_GROUP_new_by_curve_name( EC_curve_nist2nid( "P-521" ) );
  #endif

  EC_POINT* P   = EC_POINT_new( G );
  EC_POINT* Q   = EC_POINT_new( G );

  BN_CTX*   ctx = BN_CTX_new();

  BIGNUM*   x   = BN_new();
  BIGNUM*   y   = BN_new();
  BIGNUM*   k   = BN_new();

  #if   CONF( TARGET_D, CID ) &&  CONF( OBFUSCATE )
  unmask( __k,    USER( K_SIZE, CID ), USER( K_MASK, CID ) ); 
  #elif CONF( TARGET_R, CID ) || !CONF( OBFUSCATE )
  memset( __k, 0, USER( K_SIZE, CID )                      );
  #endif

  #if   CONF( TARGET_D, CID )
  BN_bin2bn( __k, USER( K_SIZE, CID ), k );
  #endif

  while( true ) {
    // 1. consume input

    char T[ BUFFER ];

    CONSUME( fscanf( stdin, "%s", T ), 1 );
    BN_hex2bn( &x, T );
    CONSUME( fscanf( stdin, "%s", T ), 1 );
    BN_hex2bn( &y, T );

    EC_POINT_set_affine_coordinates_GFp( G, P, x, y, ctx );

    #if CONF( TARGET_R, CID )
    CONSUME( fscanf( stdin, "%s", T ), 1 );
    BN_hex2bn( &k, T );
    #endif

    // 2. execute operation

    mul( G, Q, P, k, ctx );

    #if CONF( DEBUG )
    fprintf( stderr, "k   = " ); BN_print_fp( stderr, k ); fprintf( stderr, "\n" );

    EC_POINT_get_affine_coordinates_GFp( G, P, x, y, ctx );

    fprintf( stderr, "P_x = " ); BN_print_fp( stderr, x ); fprintf( stderr, "\n" );
    fprintf( stderr, "P_y = " ); BN_print_fp( stderr, y ); fprintf( stderr, "\n" );

    EC_POINT_get_affine_coordinates_GFp( G, Q, x, y, ctx );

    fprintf( stderr, "Q_x = " ); BN_print_fp( stderr, x ); fprintf( stderr, "\n" );
    fprintf( stderr, "Q_y = " ); BN_print_fp( stderr, y ); fprintf( stderr, "\n" );
    #endif

    // 3. produce output

    if( EC_POINT_is_at_infinity( G, Q ) ) {
                                fprintf( stdout, "0\n" );
                                fprintf( stdout, "0\n" );
    }
    else {
      EC_POINT_get_affine_coordinates_GFp( G, Q, x, y, ctx );

      BN_print_fp( stdout, x ); fprintf( stdout,  "\n" );
      BN_print_fp( stdout, y ); fprintf( stdout,  "\n" );
    }

    // 4. flush streams

    fflush( stdout );
    fflush( stderr );
  }

  EC_GROUP_free( G );

  EC_POINT_free( P );
  EC_POINT_free( Q );

  BN_CTX_free( ctx );

  BN_free( x );
  BN_free( y );
  BN_free( k );

  return 0;
}

# Copyright (C) 2017 Daniel Page <dan@phoo.org>
#
# Use of this source code is restricted per the CC BY-SA license, a copy of
# which can be found via http://creativecommons.org (and should be included 
# as LICENSE.txt within the associated archive or repository).

import math, random, string, libbuild.util as util, Crypto.Cipher.PKCS1_OAEP as OAEP, Crypto.PublicKey.RSA as RSA ; from Crypto.Hash import SHA, SHA256, SHA384, SHA512

def apply_fixups( args, conf ) :
  if   ( conf.get( 'challenge', 'rsa_padding' ) == 'user_hash' ) :
    conf.set( 'challenge', 0, 'rsa_padding' )    
  elif ( conf.get( 'challenge', 'rsa_padding' ) == 'user_text' ) :
    conf.set( 'challenge', 1, 'rsa_padding' )    
  elif ( conf.get( 'challenge', 'rsa_padding' ) == 'rand'      ) :
    conf.set( 'challenge', 2, 'rsa_padding' )        

  if   ( conf.get( 'hash', 'rsa_padding'      ) == 'sha1'      ) :
    conf.set( 'hash',      0, 'rsa_padding' )
  elif ( conf.get( 'hash', 'rsa_padding'      ) == 'sha256'    ) :
    conf.set( 'hash',      1, 'rsa_padding' )
  elif ( conf.get( 'hash', 'rsa_padding'      ) == 'sha384'    ) :
    conf.set( 'hash',      2, 'rsa_padding' )
  elif ( conf.get( 'hash', 'rsa_padding'      ) == 'sha512'    ) :
    conf.set( 'hash',      3, 'rsa_padding' )

def build_params( args, conf ) :
  log_N = random.randint( int( conf.get( 'log_N_min', 'rsa_padding' ) ),
                          int( conf.get( 'log_N_max', 'rsa_padding' ) ) )
  log_m = random.randint( int( conf.get( 'log_m_min', 'rsa_padding' ) ),
                          int( conf.get( 'log_m_max', 'rsa_padding' ) ) )
  log_L = random.randint( int( conf.get( 'log_L_min', 'rsa_padding' ) ),
                          int( conf.get( 'log_L_max', 'rsa_padding' ) ) )

  k = int( math.ceil( log_N / 8 ) ) ; B = 2 ** ( 8 * ( k - 1 ) )

  while ( True ) :
    p, q, N, e, d = util.keygen_rsa( log_N )

    if ( ( int( math.ceil( math.log( N, 2 ** 8 ) ) ) == k ) and ( ( 2 * B ) < N ) ) :
      break

  m = util.bytes_rand( int( math.ceil( log_m / 8 ) ) )
  L = util.bytes_rand( int( math.ceil( log_L / 8 ) ) )

  if   ( conf.get( 'challenge', 'rsa_padding' ) == 'user_hash' ) :
    m = bytearray( ( SHA.new( data = args.user ) ).digest() ) + m
  elif ( conf.get( 'challenge', 'rsa_padding' ) == 'user_text' ) :
    m = bytearray(                   args.user              ) + m

  if   ( conf.get( 'hash', 'rsa_padding' ) == 'sha1'   ) :
    H = SHA
  elif ( conf.get( 'hash', 'rsa_padding' ) == 'sha256' ) :
    H = SHA256
  elif ( conf.get( 'hash', 'rsa_padding' ) == 'sha384' ) :
    H = SHA384
  elif ( conf.get( 'hash', 'rsa_padding' ) == 'sha512' ) :
    H = SHA512

  c = bytearray( OAEP.new( RSA.construct( ( N, e, d ) ), label = L, hashAlgo = H ).encrypt( str( m ) ).zfill( k ) )

  return [ ( 'p', p, False ),
           ( 'q', q, False ),
           ( 'N', N, True  ),
           ( 'e', e, True  ),
           ( 'd', d, False ),
           ( 'L', L, True  ),
           ( 'm', m, False ),
           ( 'c', c, True  ) ]

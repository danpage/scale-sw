# Copyright (C) 2017 Daniel Page <dan@phoo.org>
#
# Use of this source code is restricted per the CC BY-SA license, a copy of
# which can be found via http://creativecommons.org (and should be included 
# as LICENSE.txt within the associated archive or repository).

import math, random, string, libbuild.util as util, Crypto.Cipher.AES as AES, Crypto.Hash.HMAC as HMAC, Crypto.Hash.SHA as SHA

def apply_fixups( args, conf ) :
  if   ( conf.get( 'challenge', 'aes_padding' ) == 'uid_hash' ) :
    conf.set( 'challenge', 0, 'aes_padding' )    
  elif ( conf.get( 'challenge', 'aes_padding' ) == 'uid_text' ) :
    conf.set( 'challenge', 1, 'aes_padding' )    
  elif ( conf.get( 'challenge', 'aes_padding' ) ==     'rand' ) :
    conf.set( 'challenge', 2, 'aes_padding' )        

def build_params( args, conf ) :
  log_m = random.randint( int( conf.get( 'log_m_min', 'aes_padding' ) ),
                          int( conf.get( 'log_m_max', 'aes_padding' ) ) )

  log_k_enc = 128 ; log_b_enc = 128 ; log_k_mac = 128

  k_enc = util.bytes_rand( log_k_enc / 8 )
  k_mac = util.bytes_rand( log_k_mac / 8 )

  iv    = util.bytes_rand( int( math.ceil( log_b_enc / 8 ) ) )
  m     = util.bytes_rand( int( math.ceil( log_m     / 8 ) ) )

  if   ( conf.get( 'challenge', 'aes_padding' ) == 'uid_hash' ) :
    m = bytearray( ( SHA.new( data = args.uid ) ).digest() ) + m
  elif ( conf.get( 'challenge', 'aes_padding' ) == 'uid_text' ) :
    m = bytearray(                   args.uid              ) + m

  tau = bytearray( HMAC.new( str( k_mac ), digestmod = SHA, msg = str( m ) ).digest() )
  rho = util.padding( m + tau, log_b_enc / 8 )

  c   = bytearray( AES.new( str( k_enc ), AES.MODE_CBC, IV = str( iv ) ).encrypt( str( m + tau + rho ) ) )

  return [ ( 'k_enc', k_enc, False ),
           ( 'k_mac', k_mac, False ),
           (   'tau',   tau, False ),
           (   'rho',   rho, False ),
           (    'iv',    iv,  True ),
           (     'm',     m, False ),
           (     'c',     c,  True ) ]

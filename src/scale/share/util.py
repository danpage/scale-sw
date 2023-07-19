# Copyright (C) 2017 Daniel Page <dan@phoo.org>
#
# Use of this source code is restricted per the CC BY-SA license, a copy of
# which can be found via http://creativecommons.org (and should be included 
# as LICENSE.txt within the associated archive or repository).

import binascii, configparser, functools, itertools, math, random

import Crypto.Util.number as number

import Crypto.Cipher.AES        as AES
import Crypto.Cipher.DES        as DES
import Crypto.Cipher.PKCS1_OAEP as OAEP

import Crypto.Hash.SHA          as SHA
import Crypto.Hash.HMAC         as HMAC

import Crypto.PublicKey.RSA     as RSA

# =============================================================================

# ...

CONF_MODE_MK   = 0
CONF_MODE_SH   = 1
CONF_MODE_ARG  = 2

CONF_TYPE_CONF = 0
CONF_TYPE_USER = 1

class conf( configparser.RawConfigParser, object ) :
  def __init__( self ) :
    super( conf, self ).__init__()

  def read( self, filename ) :
    super( conf, self ).read( filename )

  def get( self, option,      section = 'global' ) :
    return super( conf, self ).get( section, option )

  def set( self, option, val, section = 'global' ) :
    super( conf, self ).set( section, option, val )

def conf_str( mode, type, section, option, val = '' ) :
  if   ( type == CONF_TYPE_CONF ) :
    type = 'CONF'
  elif ( type == CONF_TYPE_USER ) :
    type = 'USER'

  if ( section == 'global' ) :
      id =                 option
  else :
      id = section + '_' + option

  if ( isinstance( val, list ) ) :
    if  ( mode == CONF_MODE_MK  ) :
      val = ' '.join( val )
    elif( mode == CONF_MODE_SH  ) :
      val = ' '.join( val )
    elif( mode == CONF_MODE_ARG ) :
      val = ','.join( val )  

  if ( isinstance( val, str  ) ) :
    if   ( ( val.lower() == "true"  ) or ( val.lower() == "yes" ) ) :
      val = "1" 
    elif ( ( val.lower() == "false" ) or ( val.lower() == "no"  ) ) :
      val = "0" 

  if   ( mode == CONF_MODE_MK  ) :
    return r'export SCALE_%s_%s=%s'   % ( type.upper(), id.upper(), str( val ) )
  elif ( mode == CONF_MODE_SH  ) :
    return r'export SCALE_%s_%s="%s"' % ( type.upper(), id.upper(), str( val ) )
  elif ( mode == CONF_MODE_ARG ) :
    return r'-DSCALE_%s_%s="%s"'      % ( type.upper(), id.upper(), str( val ) )

# ...

def xopen( filename, mode ) :
  fd = open( filename, mode ) ; pr = functools.partial( print, file = fd ) ; return ( fd, pr )

# convert a (structured) octet string to/from a (raw) byte string

def octetstr2str( x ) :
  t = x.split( ':' ) ; n = int( t[ 0 ], 16 ) ; x = binascii.a2b_hex( t[ 1 ] )

  if( n != len( x ) ) :
    raise ValueError
  else :
    return x

def str2octetstr( x ) :
  return ( '%02X' % ( len( x ) ) ) + ':' + ( binascii.b2a_hex( x ).decode( 'ascii' ) )

# convert a byte string to/from a byte sequence

def str2seq( x ) :
  return          [ ord( t ) for t in x ]

def seq2str( x ) :
  return ''.join( [ chr( t ) for t in x ] )

# convert an integer to/from a little- or big-endian base-b sequence

LE = +1 ; BE = -1

def int2seq( x, b, endian = +1 ) :
  t = []

  while ( x != 0 ) :
    if   ( endian == LE ) : 
      t = t + [ x % b ]
    elif ( endian == BE ) : 
      t = [ x % b ] + t

    x = x // b

  return t

def seq2int( x, b, endian = +1 ) :
  if   ( endian == LE ) :
    x = enumerate(           x   )
  elif ( endian == BE ) :
    x = enumerate( reversed( x ) )

  return sum( [ t * ( b ** i ) for ( i, t ) in x ] )

# take a list of lists, and flatten it into a list, i.e., concatenate all
# the sub-lists together

def flatten( xs ) :
  return list( itertools.chain.from_iterable( xs ) )

# generate an array of n bytes, each of whose value is either zero or 
# sampled randomly

def bytes_rand( n, xs = range( 0, 256 ) ) :
  return bytes( [ random.choice( xs ) for i in range( n ) ] )

def bytes_zero( n ) :
  return bytes( [ 0                   for i in range( n ) ] )

# compute padding required for x, formed of b - ( l mod b ) elements 
# each with the value b - ( l mod b )

def padding( x, b ) :
  t = b - ( len( x ) % b ) ; return bytes( [ t ] * t )

# key generation for RSA with (optionally) constrained private exponent

def keygen_rsa( log_N, log_d = None ) :
  f = lambda n : bytes( [ random.getrandbits( 8 ) for i in range( n ) ] )

  p = number.getPrime( math.ceil( log_N / 2 ), randfunc = f )
  q = number.getPrime( math.ceil( log_N / 2 ), randfunc = f )

  N = p * q ; phi = ( p - 1 ) * ( q - 1 )

  if ( log_d == None ) :
    while ( True ) :
      e = number.getRandomRange( 0,          N, randfunc = f )

      if ( number.GCD( e, phi ) == 1 ) :
        d = number.inverse( e, phi ) ; break

  else :
    while ( True ) :
      d = number.getRandomRange( 0, 2 ** log_d, randfunc = f )

      if ( number.GCD( d, phi ) == 1 ) :
        e = number.inverse( d, phi ) ; break

  return p, q, N, e, d

# =============================================================================

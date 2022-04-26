# Copyright (C) 2017 Daniel Page <dan@phoo.org>
#
# Use of this source code is restricted per the CC BY-SA license, a copy of
# which can be found via http://creativecommons.org (and should be included 
# as LICENSE.txt within the associated archive or repository).

import math, random, string, libbuild.util as util

def apply_fixups( args, conf ) :
  if   ( conf.get( 'version', 'aes_power' ) == 'ecb_enc' ) :
    conf.set( 'version', 0, 'aes_power' )    
  elif ( conf.get( 'version', 'aes_power' ) == 'ecb_dec' ) :
    conf.set( 'version', 1, 'aes_power' )    
  elif ( conf.get( 'version', 'aes_power' ) == 'xts'     ) :
    conf.set( 'version', 2, 'aes_power' )    

def build_params( args, conf ) :
  if   ( conf.get( 'version', 'aes_power' ) == 'ecb_enc' ) :
    k   =           util.bytes_rand( 128 // 8 )
  elif ( conf.get( 'version', 'aes_power' ) == 'ecb_dec' ) :
    k   =           util.bytes_rand( 128 // 8 )
  elif ( conf.get( 'version', 'aes_power' ) == 'xts'     ) :
    k_1 =           util.bytes_rand( 128 // 8 )
    k_2 =           util.bytes_rand( 128 // 8 )

    k   = k_1 + k_2

  return [ ( 'k', k, False ) ]






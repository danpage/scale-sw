# Copyright (C) 2017 Daniel Page <dan@phoo.org>
#
# Use of this source code is restricted per the CC BY-SA license, a copy of
# which can be found via http://creativecommons.org (and should be included 
# as LICENSE.txt within the associated archive or repository).

import math, random, string, libbuild.util as util

def apply_fixups( args, conf ) :
  if   ( conf.get( 'alphabet', 'password' ) == 'binary' ) :
    conf.set( 'alphabet', 0, 'password' )
  elif ( conf.get( 'alphabet', 'password' ) == 'ascii'  ) :
    conf.set( 'alphabet', 1, 'password' )

def build_params( args, conf ) :
  len_P = random.randint( int( conf.get( 'len_P_min', 'password' ) ),
                          int( conf.get( 'len_P_max', 'password' ) ) )

  if   ( conf.get( 'alphabet', 'password' ) == 'binary' ) :
    P = util.bytes_rand( len_P                                                ) + bytes( '\x00'.encode( 'ascii' ) )
  elif ( conf.get( 'alphabet', 'password' ) == 'ascii'  ) :
    P = util.bytes_rand( len_P, xs = string.ascii_lowercase.encode( 'ascii' ) ) + bytes( '\x00'.encode( 'ascii' ) )

  return [ ( 'P', P, False ) ]

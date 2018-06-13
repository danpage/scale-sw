# Copyright (C) 2017 Daniel Page <dan@phoo.org>
#
# Use of this source code is restricted per the CC BY-SA license, a copy of
# which can be found via http://creativecommons.org (and should be included 
# as LICENSE.txt within the associated archive or repository).

import math, random, string, libbuild.util as util

def apply_fixups( args, conf ) :
  if   ( conf.get( 'alphabet', 'demo' ) == 'binary' ) :
    conf.set( 'alphabet', 0, 'demo' )
  elif ( conf.get( 'alphabet', 'demo' ) == 'ascii'  ) :
    conf.set( 'alphabet', 1, 'demo' )

def build_params( args, conf ) :
  len_P = random.randint( int( conf.get( 'len_P_min', 'demo' ) ),
                          int( conf.get( 'len_P_max', 'demo' ) ) )

  if   ( conf.get( 'alphabet', 'demo' ) == 'binary' ) :
    P = util.bytes_rand( len_P                                 ) + '\x00'
  elif ( conf.get( 'alphabet', 'demo' ) == 'ascii'  ) :
    P = util.bytes_rand( len_P, bytes = string.ascii_lowercase ) + '\x00'

  return [ ( 'P', P, False ) ]

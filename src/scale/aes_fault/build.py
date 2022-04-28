# Copyright (C) 2017 Daniel Page <dan@phoo.org>
#
# Use of this source code is restricted per the CC BY-SA license, a copy of
# which can be found via http://creativecommons.org (and should be included 
# as LICENSE.txt within the associated archive or repository).

import util, math, random, string

# =============================================================================

def apply_fixups( args, conf ) :
  pass

def build_params( args, conf ) :
  return [ ( 'k', util.bytes_rand( 128 // 8 ), False ) ]

# =============================================================================

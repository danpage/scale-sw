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
  log_N = random.randint( int( conf.get( 'log_N_min', 'rsa_timing' ) ),
                          int( conf.get( 'log_N_max', 'rsa_timing' ) ) )
  log_d = random.randint( int( conf.get( 'log_d_min', 'rsa_timing' ) ),
                          int( conf.get( 'log_d_max', 'rsa_timing' ) ) )

  p, q, N, e, d = util.keygen_rsa( log_N, log_d )

  return [ ( 'p', p, False ),
           ( 'q', q, False ),
           ( 'N', N, True  ),
           ( 'e', e, True  ),
           ( 'd', d, False ) ]

# =============================================================================

# Copyright (C) 2017 Daniel Page <dan@phoo.org>
#
# Use of this source code is restricted per the CC BY-SA license, a copy of
# which can be found via http://creativecommons.org (and should be included 
# as LICENSE.txt within the associated archive or repository).

# This script is a minimum working example of an attacker in the rsa_timing 
# challenge: it acts as a more general illustration of how to interact with 
# the executable targets in all challenges, in terms of the steps involved.

import random, subprocess, sys

if ( __name__ == '__main__' ) :
  # step 1: execute attack target

  target     = subprocess.Popen( args = sys.argv[ 1 ], stdout = subprocess.PIPE, 
                                                       stdin  = subprocess.PIPE )
     
  target_out = target.stdout
  target_in  = target.stdin

  # step 2: load content from configuration file
     
  conf       = open( sys.argv[ 2 ], 'r' )
     
  N          = int( conf.readline(), 16 )
  e          = int( conf.readline(), 16 )
     
  conf.close()
     
  # step 3: generate a random ciphertext

  c          = random.randint( 0, N - 1 )
     
  # step 4 : send    input to    attack target

  target_in.write( '%X\n' % ( c ) ) ; target_in.flush()
     
  # step 5 : receive output from attack target

  t          = int( target_out.readline().strip(), 10 )
  m          = int( target_out.readline().strip(), 16 )

  # step 6 : dump configuration, inputs, and outputs

  print 'N = %X' % N
  print 'e = %X' % e
  print 'c = %X' % c
  print 't = %d' % t
  print 'm = %X' % m

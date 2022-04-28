# Copyright (C) 2017 Daniel Page <dan@phoo.org>
#
# Use of this source code is restricted per the CC BY-SA license, a copy of
# which can be found via http://creativecommons.org (and should be included 
# as LICENSE.txt within the associated archive or repository).

import math, random, string, libbuild.util as util

def apply_fixups( args, conf ) :
  if   ( conf.get( 'curve', 'ecc_invalid' ) == 'nist-p-192' ) :
    conf.set( 'curve', 0, 'ecc_invalid' )    
  elif ( conf.get( 'curve', 'ecc_invalid' ) == 'nist-p-224' ) :
    conf.set( 'curve', 1, 'ecc_invalid' )    
  elif ( conf.get( 'curve', 'ecc_invalid' ) == 'nist-p-256' ) :
    conf.set( 'curve', 2, 'ecc_invalid' )        
  elif ( conf.get( 'curve', 'ecc_invalid' ) == 'nist-p-384' ) :
    conf.set( 'curve', 3, 'ecc_invalid' )        
  elif ( conf.get( 'curve', 'ecc_invalid' ) == 'nist-p-521' ) :
    conf.set( 'curve', 4, 'ecc_invalid' )        

def build_params( args, conf ) :
  if   ( conf.get( 'curve', 'ecc_invalid' ) == 'nist-p-192' ) :
    log_k = 192
  elif ( conf.get( 'curve', 'ecc_invalid' ) == 'nist-p-224' ) :
    log_k = 224
  elif ( conf.get( 'curve', 'ecc_invalid' ) == 'nist-p-256' ) :
    log_k = 256
  elif ( conf.get( 'curve', 'ecc_invalid' ) == 'nist-p-384' ) :
    log_k = 384
  elif ( conf.get( 'curve', 'ecc_invalid' ) == 'nist-p-521' ) :
    log_k = 521

  return [ ( 'k', util.bytes_rand( math.ceil( log_k / 8 ) ), False ) ]

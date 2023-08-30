# Copyright (C) 2017 Daniel Page <dan@phoo.org>
#
# Use of this source code is restricted per the CC BY-SA license, a copy of
# which can be found via http://creativecommons.org (and should be included 
# as LICENSE.txt within the associated archive or repository).

import util, argparse, binascii, braceexpand, copy, configparser, glob, os, pickle, random, sys, textwrap

# =============================================================================

def apply_fixups() :
  # apply global fixups

  def expand( x, globs = False ) :
    rs = [] 

    ts = x.split( ' ' )
    ts =               map( lambda t : t.strip(),                            ts )
    ts = util.flatten( map( lambda t : braceexpand.braceexpand( t.strip() ), ts ) )
    ts =               map( lambda t : os.path.expandvars( t ),              ts )

    if ( globs ) :
      ts = util.flatten( map( lambda t : glob.glob( t ), ts ) )

    for t in ts :
      if ( os.path.isfile( t ) ) :
        rs.extend( [ x.strip( '\n' ) for x in open( t, 'r' ).readlines() ] )
      else :
        rs.append( t )

    return rs

  conf.set( 'users',      expand( conf.get( 'users'      ), globs = False ) )
  conf.set( 'challenges', expand( conf.get( 'challenges' ), globs = True  ) )
  conf.set( 'seed',       expand( conf.get( 'seed'       ), globs = False ) )

  # apply local  fixups

  for challenge in conf.get( 'challenges' ) :
    if ( not os.path.isfile( os.path.join( challenge, 'build.py' ) ) ) :
      continue

    # update module path; perform import
    sys.path.append( challenge ) ; import build
    # run challenge-specific actions
    build.apply_fixups( args, conf )
    # update module path
    sys.path.pop() 
    # update module name->object map
    del sys.modules[ 'build' ]

# -----------------------------------------------------------------------------

if ( __name__ == '__main__' ) :
  # parse command-line args

  parser = argparse.ArgumentParser()

  parser.add_argument( '--output', action = 'store', dest = 'output'                                  )

  parser.add_argument( '--conf',   action = 'store', dest =   'conf'                                  )
  parser.add_argument( '--mode',   action = 'store', dest =   'mode', choices = [ 'mk', 'sh', 'arg' ] )

  args = parser.parse_args()

  # parse configuration file

  conf = util.conf() ; conf.read( args.conf )

  # execute

  apply_fixups()

  ( fd, pr ) = util.xopen( os.path.join( args.output, 'conf' + '.' + args.mode ), 'w' )

  for section in conf.sections() :
    for option in conf.options( section ) :
      val = conf.get( option, section )

      if   ( args.mode == 'mk'  ) :
        pr( util.conf_str( util.CONF_MODE_MK,  util.CONF_TYPE_CONF, section, option, val ) )
      elif ( args.mode == 'sh'  ) :
        pr( util.conf_str( util.CONF_MODE_SH,  util.CONF_TYPE_CONF, section, option, val ) )
      elif ( args.mode == 'arg' ) :
        pr( util.conf_str( util.CONF_MODE_ARG, util.CONF_TYPE_CONF, section, option, val ) )

  fd.close()

# =============================================================================

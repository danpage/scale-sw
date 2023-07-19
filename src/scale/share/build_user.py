# Copyright (C) 2017 Daniel Page <dan@phoo.org>
#
# Use of this source code is restricted per the CC BY-SA license, a copy of
# which can be found via http://creativecommons.org (and should be included 
# as LICENSE.txt within the associated archive or repository).

import util, argparse, binascii, braceexpand, copy, configparser, glob, os, pickle, random, sys, textwrap, Crypto.Hash.SHA as SHA

# =============================================================================

def build_conf() :
  # build parameters for this user

  if ( not os.path.isfile( os.path.join( args.path, 'build.py' ) ) ) :
    return
  else :
    sys.path.append( args.path ) ; import build 

  params = build.build_params( args, conf )

  # dump all parameters as machine-readable blob

  ( fd, pr ) = util.xopen( os.path.join( args.output, args.uid + '.conf_bin' ), 'wb' )

  pickle.dump( params, fd )

  fd.close()

  # dump all parameters as   human-readable text

  ( fd, pr ) = util.xopen( os.path.join( args.output, args.uid + '.conf_txt' ), 'w'  )

  for ( pid, val, pub ) in params :
    if   ( type( val ) == int   ) :
      pr( '%s = %X_{(16)}' % (          ( pid ),                    val           ) )
      pr( '%s = %d_{(10)}' % ( ' ' * len( pid ),                    val           ) )
    elif ( type( val ) == bytes ) :
      pr( '%s = %s'        % (       str( pid ),                    val           ) )
      pr( '%s = %s'        % ( ' ' * len( pid ), util.str2octetstr( val ).upper() ) )

  fd.close()

  # dump all parameters as machine-readable macro definitions

  ( fd, pr ) = util.xopen( os.path.join( args.output, args.uid + '.conf_arg' ), 'w'  )

  for ( pid, val, pub ) in params :
    if   ( type( val ) == int   ) :
      val = util.int2seq( val, 2 ** 8, util.LE ) ; 
    elif ( type( val ) == bytes ) :
      pass

    if ( conf.get( 'obfuscate' ) == 'true' ) :
      m = random.randint( 0, 2 ** 8 - 1 )
    else :
      m = 0x00

    tn = len( val ) ; t = ','.join( [ "0x%02X" % ( b ^ m ) for b in val ] ) ; m = '0x%02X' % ( m )

    pr( util.conf_str( util.CONF_MODE_ARG, util.CONF_TYPE_USER, args.cid, pid + '_' + 'SIZE', str( tn ) ) )
    pr( util.conf_str( util.CONF_MODE_ARG, util.CONF_TYPE_USER, args.cid, pid + '_' + 'DATA', str( t  ) ) )
    pr( util.conf_str( util.CONF_MODE_ARG, util.CONF_TYPE_USER, args.cid, pid + '_' + 'MASK', str( m  ) ) )

  fd.close()

  # dump public parameters only, for distribution to user

  ( fd, pr ) = util.xopen( os.path.join( args.output, args.uid + '.conf'     ), 'w'  )

  for ( pid, val, pub ) in params :
    if ( not pub ) :
      continue

    if   ( type( val ) == int   ) :
      pr( '%X' % (                    val           ) )
    elif ( type( val ) == bytes ) :
      pr( '%s' % ( util.str2octetstr( val ).upper() ) )

  fd.close()

def build_exam() :
  qdb = dict() ; qid = None

  # read question database, i.e.,
  # 
  # - read lines of input, ignoring empty and commented lines,
  # - split line into a question identifier and text, allowing 
  #   identifier to "roll forward" into subsequent lines which
  #   lack one,
  # - update entry relating to question identifier with text

  fd = open( os.path.join( args.path, 'build.db' ), 'r' )
  
  for line in fd :
    line = line.strip()
  
    if ( ( line == '' ) or ( line.startswith( '%' ) ) ) :
      continue
  
    line = [ t.strip() for t in line.split( '!' ) ]

    if ( line[ 0 ] != '' ) :
      ( i, t ) = line[ 0 ].split( '.' ) ; qid = ( int( i ), t )

    text = line[ 1 ]
  
    if ( not qid in qdb ) :
      qdb[ qid ] =                    text
    else :
      qdb[ qid ] = qdb[ qid ] + ' ' + text

  fd.close()
  
  # select and dump required then optional questions

  ( fd, pr ) = util.xopen( os.path.join( args.output, args.uid + '.exam' ), 'w' )

  n = random.randint( int( conf.get( 'optional_min' ) ),
                      int( conf.get( 'optional_max' ) ) )

  qs_r = [ ( i, t ) for ( i, t ) in qdb if t == 'r' ]
  qs_o = [ ( i, t ) for ( i, t ) in qdb if t == 'o' ]

  if ( len( qs_o ) >= n ) :
    qs = qs_r + random.sample( qs_o, n )
  else :
    qs = qs_r +              ( qs_o    )

  for ( i, t ) in sorted( qs, key = lambda x : x[ 0 ] ) :
    w = textwrap.TextWrapper( initial_indent = 'Q.%d ' % ( i ), subsequent_indent = ' ' * 4, width = 75, fix_sentence_endings = True )

    for line in w.wrap( qdb[ ( i, t ) ] ) :
      pr( line )

  fd.close()

# -----------------------------------------------------------------------------

if ( __name__ == '__main__' ) :
  # parse command-line args

  parser = argparse.ArgumentParser()

  parser.add_argument( '--output', action = 'store', dest = 'output'                               )

  parser.add_argument( '--conf',   action = 'store', dest =   'conf'                               )
  parser.add_argument( '--mode',   action = 'store', dest =   'mode', choices = [ 'conf', 'exam' ] )

  parser.add_argument( '--path',   action = 'store', dest =   'path'                               )

  parser.add_argument( '--cid',    action = 'store', dest =    'cid'                               )
  parser.add_argument( '--uid',    action = 'store', dest =    'uid'                               )

  args = parser.parse_args()

  # parse configuration file

  conf = util.conf() ; conf.read( args.conf )

  # execute

  seed = conf.get( 'seed' ).strip( ' ' )

  if ( seed == '' ) :
    seed = int( SHA.new( data = ( args.cid + args.uid ).encode( 'ascii' ) ).hexdigest(), 16 )
  else :
    seed = int( SHA.new( data = ( args.cid + args.uid ).encode( 'ascii' ) ).hexdigest(), 16 ) + int( seed )

  random.seed( seed )

  if   ( args.mode == 'conf' ) :
    build_conf()
  elif ( args.mode == 'exam' ) :
    build_exam()

# =============================================================================

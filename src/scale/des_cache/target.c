/* Copyright (C) 2017 Daniel Page <dan@phoo.org>
 *
 * Use of this source code is restricted per the CC BY-SA license, a copy of
 * which can be found via http://creativecommons.org (and should be included 
 * as LICENSE.txt within the associated archive or repository).
 */

#include "target.h"

d4cache* cache_model_Ln   = NULL; //    memory
d4cache* cache_model_L2   = NULL; // L2 cache
d4cache* cache_model_L1   = NULL; // L1 cache

#if CONF( DEBUG )
    int  cache_trace_used =    0;
    int  cache_trace_size =    0;
    int* cache_trace      = NULL;
#endif
  
d4cache* level( d4cache* p, char* id, int f, int c, int w, int N ) {
  d4cache* t = d4new( p );

  t->name                  = id;
  t->flags                 =  f;

  t->lg2size               =  c; 
  t->lg2blocksize          =  w;  
  t->lg2subblocksize       =  w;  
  t->assoc                 =  N;

  t->replacementf          = &d4rep_lru;
  t->prefetchf             = &d4prefetch_none;
  t->wallocf               = &d4walloc_always;
  t->wbackf                = &d4wback_never;

  t->prefetch_distance     =  1;
  t->prefetch_abortpercent =  1;

  t->name_replacement      = "";
  t->name_prefetch         = "";
  t->name_walloc           = "";
  t->name_wback            = "";

  return t;
}

uint32_t cache_ld( uint32_t* x, int i              ) {
  d4memref t;

  t.address    = ( D4ADDR )( x ) + ( i * SIZEOF( uint32_t ) );
  t.size       =                   (     SIZEOF( uint32_t ) );
  t.accesstype = D4XREAD;  

  int miss_b = cache_model_L1->miss[ D4XREAD ];
  d4ref( cache_model_L1, t );
  int miss_a = cache_model_L1->miss[ D4XREAD ];

  #if CONF( DEBUG )
  int stat_m = ( int )(                                    cache_model_L1->miss[ D4XREAD ] );
  int stat_h = ( int )( cache_model_L1->fetch[ D4XREAD ] - cache_model_L1->miss[ D4XREAD ] );
  int stat_a = ( int )( cache_model_L1->fetch[ D4XREAD ]                                   );

  if( ( cache_trace == NULL ) || ( cache_trace_used == cache_trace_size ) ) {
    cache_trace = ( int* )( realloc( cache_trace, ( cache_trace_size += CACHE_TRACE_SIZE_INC ) * SIZEOF( int ) ) );
  }

  cache_trace[ cache_trace_used++ ] = ( miss_a != miss_b ) ? CACHE_TRACE_STAT_M : CACHE_TRACE_STAT_H;
  #endif  

  return x[ i ];
}

uint32_t cache_st( uint32_t* x, int i , uint32_t y ) {
  d4memref t;

  t.address    = ( D4ADDR )( x ) + ( i * SIZEOF( uint32_t ) );
  t.size       =                   (     SIZEOF( uint32_t ) );
  t.accesstype = D4XWRITE;

  int miss_b = cache_model_L1->miss[ D4XREAD ];
  d4ref( cache_model_L1, t );
  int miss_a = cache_model_L1->miss[ D4XREAD ];

  #if CONF( DEBUG )
  if( ( cache_trace == NULL ) || ( cache_trace_used == cache_trace_size ) ) {
    cache_trace = ( int* )( realloc( cache_trace, ( cache_trace_size += CACHE_TRACE_SIZE_INC ) * SIZEOF( int ) ) );
  }

  cache_trace[ cache_trace_used++ ] = ( miss_a != miss_b ) ? CACHE_TRACE_STAT_M : CACHE_TRACE_STAT_H;
  #endif  

  return x[ i ] = y;
}

#include "D3DES.H"
#include "D3DES.C"

int main( int argc, char* argv[] ) {
  uint8_t c[ DES_NB ], m[ DES_NB ], k[] = { USER( K_DATA, CID ) };

  #if   CONF( TARGET_D, CID ) &&  CONF( OBFUSCATE )
  unmask( k,    USER( K_SIZE, CID ), USER( K_MASK, CID ) );
  #elif CONF( TARGET_R, CID ) || !CONF( OBFUSCATE )
  memset( k, 0, USER( K_SIZE, CID )                      );
  #endif

  cache_model_Ln = level( NULL,           CACHE_MODEL_LN_ID, D4F_MEM, 
                                          CACHE_MODEL_LN_CAPACITY, 
                                          CACHE_MODEL_LN_SUBWORDS, 
                                          CACHE_MODEL_LN_ASSOC );
  cache_model_L2 = level( cache_model_Ln, CACHE_MODEL_L2_ID, D4F_CCC, 
                                          CACHE_MODEL_L2_CAPACITY, 
                                          CACHE_MODEL_L2_SUBWORDS, 
                                          CACHE_MODEL_L2_ASSOC );
  cache_model_L1 = level( cache_model_L2, CACHE_MODEL_L1_ID, D4F_CCC, 
                                          CACHE_MODEL_L1_CAPACITY, 
                                          CACHE_MODEL_L1_SUBWORDS, 
                                          CACHE_MODEL_L1_ASSOC );
  if( d4setup() != 0 ) {
    abort();
  }

  while( true ) {
    int lambda;

    // 1. consume input

    CONSUME( octetstr_rd( stdin, m, DES_NB ), DES_NB );  

    #if CONF( TARGET_R, CID )
    CONSUME( octetstr_rd( stdin, k, DES_NK ), DES_NK );  
    #endif

    // 2. execute operation

    d4invalidate( cache_model_L1, NULL, true );

    cache_model_L1->fetch[ D4XREAD ] = 0;
    cache_model_L1->miss [ D4XREAD ] = 0;

    #if CONF( DEBUG )
    cache_trace_used = 0;
    #endif

    deskey( k, EN0 ); des( m, c );

    // max misses ~= 64
    // min misses ~= 43
    //            => 1430 < \lambda < 1640 cycles

    int stat_m = ( int )(                                    cache_model_L1->miss[ D4XREAD ] );
    int stat_h = ( int )( cache_model_L1->fetch[ D4XREAD ] - cache_model_L1->miss[ D4XREAD ] );
    int stat_a = ( int )( cache_model_L1->fetch[ D4XREAD ]                                   );

    lambda = 1000 + ( 10 * stat_m ); 

    #if ( ( CONF( NOISE_MAX, CID ) - CONF( NOISE_MIN, CID ) ) > 0 )
    lambda += CONF( NOISE_MIN, CID ) + ( prng_32() % ( CONF( NOISE_MAX, CID ) - CONF( NOISE_MIN, CID ) ) );
    #endif

    #if CONF( DEBUG )
    fprintf( stderr, "&SP1     = %p\n", &SP1 );
    fprintf( stderr, "&SP2     = %p\n", &SP2 );
    fprintf( stderr, "&SP3     = %p\n", &SP3 );
    fprintf( stderr, "&SP4     = %p\n", &SP4 );
    fprintf( stderr, "&SP5     = %p\n", &SP5 );
    fprintf( stderr, "&SP6     = %p\n", &SP6 );
    fprintf( stderr, "&SP7     = %p\n", &SP7 );
    fprintf( stderr, "&SP8     = %p\n", &SP8 );  

    fprintf( stderr, "trace    = " );

    for( int i = 0; i < cache_trace_used; i++ ) {
      int t = cache_trace[ i ];

      switch( t ) {
        case CACHE_TRACE_STAT_M : fprintf( stderr, "M" ); break;
        case CACHE_TRACE_STAT_H : fprintf( stderr, "H" ); break;
      }
    }

    fprintf( stderr, "\n" );

    fprintf( stderr, "misses   = %d\n", stat_m );  
    fprintf( stderr, "hits     = %d\n", stat_h );
    fprintf( stderr, "accesses = %d\n", stat_a );

    fprintf( stderr, "c = " ); octetstr_wr( stderr, c, DES_NB );
    fprintf( stderr, "m = " ); octetstr_wr( stderr, m, DES_NB );
    fprintf( stderr, "k = " ); octetstr_wr( stderr, k, DES_NK );
    #endif

    // 3. produce output

    fprintf( stdout, "%d\n", lambda ); octetstr_wr( stdout, c, DES_NB );

    // 4. flush streams

    fflush( stdout );
    fflush( stderr );
  }

  return 0;
}

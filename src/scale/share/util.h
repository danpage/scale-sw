/* Copyright (C) 2017 Daniel Page <dan@phoo.org>
 *
 * Use of this source code is restricted per the CC BY-SA license, a copy of
 * which can be found via http://creativecommons.org (and should be included 
 * as LICENSE.txt within the associated archive or repository).
 */

#ifndef __UTIL_H
#define __UTIL_H

#include  <stdarg.h>
#include <stdbool.h>
#include  <stdint.h>
#include   <stdio.h>
#include  <stdlib.h>

#include  <assert.h>
#include   <ctype.h>
#include  <limits.h>
#include  <setjmp.h>
#include  <string.h>

/** @brief Generic length for buffers for line-oriented input. */
#define BUFFER ( 1024 )

/** @defgroup SELECT
  * 
  * These macros act as support to others, offering a mechanism to select
  * some i-th of n named and m unnamed arguments: the idea is that if the
  * named arguments are supplied from a variadic list, the length of said
  * list determines i and hence which argument is selected.
  * @{ */
/** @brief 2-argument SELECT macro. */
#define __SELECT2( __1, __2,           __F, ... ) __F
/** @brief 3-argument SELECT macro. */
#define __SELECT3( __1, __2, __3,      __F, ... ) __F
/** @brief 4-argument SELECT macro. */
#define __SELECT4( __1, __2, __3, __4, __F, ... ) __F
/** @} */

/** @defgroup CONFIG
  *
  * These macros offer an abstraction on top of parameters distilled from
  * Makefile.conf into pre-processor definitions and thus symbols.  The
  * basic idea is to make the source code less verbose, and avoid having
  * to hard-code a challenge ID.  There are two versions, offering access
  * to 
  * a) the global configuration (via CONF), which is  generic to all
  *    material                 (e.g., block cipher parameters),
  *    and
  * b) the user   configuration (via USER), which is specific to the 
  *    material being generated (e.g., block cipher key material).
  * @{ */
/** @brief 1-argument CONF macro, i.e.,         without challenge ID.
  * @param x parameter ID */
#define __CONF1( x    ) SCALE_CONF_           ## x
/** @brief 1-argument USER macro, i.e.,         without challenge ID.
  * @param x parameter ID */
#define __USER1( x    ) SCALE_USER_           ## x
/** @brief 2-argument CONF macro, i.e., with            challenge ID.
  * @param x parameter ID
  * @param y challenge ID */
#define __CONF2( x, y ) SCALE_CONF_ ## y ## _ ## x
/** @brief 2-argument USER macro, i.e., with            challenge ID.
  * @param x parameter ID
  * @param y challenge ID */
#define __USER2( x, y ) SCALE_USER_ ## y ## _ ## x
/** @brief n-argument CONF macro, i.e., with or without challenge ID. */
#define   CONF( ... ) __SELECT2( __VA_ARGS__, __CONF2, __CONF1 )( __VA_ARGS__ )
/** @brief n-argument USER macro, i.e., with or without challenge ID. */
#define   USER( ... ) __SELECT2( __VA_ARGS__, __USER2, __USER1 )( __VA_ARGS__ )
/** @} */

/** @defgroup CONSUME
  * 
  * @{ */
/** @brief 
  * @param x */
#define __CONSUME1(x  )                              \
      (x);         {                                 \
    if( feof( stdin ) ) { break; }                   \
   }
/** @brief 
  * @param x
  * @param y */
#define __CONSUME2(x,y)                              \
  if( (x) != (y) ) {                                 \
    if( feof( stdin ) ) { break; } else { abort(); } \
  }                                                  
#define   CONSUME( ... ) __SELECT2( __VA_ARGS__, __CONSUME2, __CONSUME1 )( __VA_ARGS__ )
/** @} */

/** @defgroup HEX
  *
  * These macros use the pre-processor concatination operator to turn
  * x, a string of hexadecimal digits, into a C hexadecimal literal.
  * @{ */
#define __HEX(x) 0x ## x
#define   HEX(x) __HEX(x)
/** @} */

/** @defgroup STR
  *
  * These macros use the pre-processor stringify     operator to turn
  * x                                  into a C string      literal.
  * @{ */
#define __STR(x)      #x
#define   STR(x) __STR(x)
/** @} */

/** @brief Compute the size of x in bytes. */
#define SIZEOF(x) ( sizeof(x)     )
/** @brief Compute the size of x in  bits. */
#define BITSOF(x) ( sizeof(x) * 8 )

#define MAX(x,y) ( ( (x) > (y) ) ? (x) : (y) )
#define MIN(x,y) ( ( (x) < (y) ) ? (x) : (y) )
#define ABS(x) ( ( (x) < 0 ) ? -(x) : +(x) )

#define CEIL(x,y) ( ( (x) / (y) ) + ( ( (x) % (y) ) ? 1 : 0 ) )

/** @brief  Shift-left  n-bit value x by y bits. */
#define SHL(x,y,n) ( (x) << (y) )
/** @brief  Shift-right n-bit value x by y bits. */
#define SHR(x,y,n) ( (x) >> (y) )
/** @brief Rotate-left  n-bit value x by y bits. */
#define RTL(x,y,n) ( ( (x) << (y) ) | ( (x) >> ( (n) - (y) ) ) )
/** @brief Rotate-right n-bit value x by y bits. */
#define RTR(x,y,n) ( ( (x) >> (y) ) | ( (x) << ( (n) - (y) ) ) )

// convert hexadecimal character to integer value
int  xtoi( char x );
// convert integer value to hexadecimal character
char itox( int  x );

// unmask an n-element sequence x using mask m
extern void unmask( uint8_t* x, int n, uint8_t m );

// check whether x is n zero bytes or not, ~= memcmp( x, { 0, 0, ..., 0 }, n )
extern bool memeqz( const uint8_t* x, int n );

// sample an output from the PRNG
extern uint8_t  prng();

extern uint16_t prng_16();
extern uint32_t prng_32();

// strip leading and trailing whitespace from string x
extern char* strip( char* x );

//  read (at most) n_r-element sequence r from fd, return elements read
extern int  octetstr_rd( FILE* fd,       uint8_t* r, int n_r );
// write           n_x-element sequence x to   fd
extern void octetstr_wr( FILE* fd, const uint8_t* x, int n_x );

#endif

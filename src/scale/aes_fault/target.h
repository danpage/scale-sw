/* Copyright (C) 2017 Daniel Page <dan@phoo.org>
 *
 * Use of this source code is restricted per the CC BY-SA license, a copy of
 * which can be found via http://creativecommons.org (and should be included 
 * as LICENSE.txt within the associated archive or repository).
 */

#ifndef __TARGET_H
#define __TARGET_H

// ============================================================================

#include "util.h"
#include "conv.h"
#include  "aes.h"

// ----------------------------------------------------------------------------

#define FAULT_FUNCTION_KEY  (    0 )
#define FAULT_FUNCTION_SUB  (    1 )
#define FAULT_FUNCTION_ROW  (    2 )
#define FAULT_FUNCTION_MIX  (    3 )

#define FAULT_PRIORITY_PRE  (    0 )
#define FAULT_PRIORITY_POST (    1 )

typedef struct __fault_t {
  bool enable;    // enable/disable fault injection
  
  int  round;     //          round number
  int  function;  //          round function
  int  priority;  // pre/post round function

  int  row;       // state matrix row    index
  int  col;       // state matrix column index
} fault_t;

// ============================================================================

#endif


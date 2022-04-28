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

#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/hmac.h>
#include <openssl/aes.h>

// ----------------------------------------------------------------------------

#define RESULT_CODE_0 ( +0 ) /* success                    */
#define RESULT_CODE_1 ( +1 ) /* failed padding check       */
#define RESULT_CODE_2 ( +2 ) /* failed MAC     check       */
#define RESULT_CODE_3 ( +3 ) /* failure for unknown reason */

// ============================================================================

#endif

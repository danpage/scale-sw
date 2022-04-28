/* Copyright (C) 2017 Daniel Page <dan@phoo.org>
 *
 * Use of this source code is restricted per the CC BY-SA license, a copy of
 * which can be found via http://creativecommons.org (and should be included 
 * as LICENSE.txt within the associated archive or repository).
 */

#ifndef __TARGET_H
#define __TARGET_H

#include <target/util.h>
#include <target/conv.h>

#include <openssl/evp.h>
#include <openssl/sha.h>

#include <gmp.h>

#define RESULT_CODE_0 ( +0 ) /* success                          */
#define RESULT_CODE_1 ( +1 ) /* failed zero octet check          */
#define RESULT_CODE_2 ( +2 ) /* failed integrity  check          */
#define RESULT_CODE_3 ( +3 ) /* RSA encrypt out of range         */
#define RESULT_CODE_4 ( +4 ) /* RSA decrypt out of range         */
#define RESULT_CODE_5 ( +5 ) /* OAEP encrypt length check failed */
#define RESULT_CODE_6 ( +6 ) /* OAEP decrypt length check failed */
#define RESULT_CODE_7 ( +7 ) /* OAEP decrypt length check failed */
#define RESULT_CODE_8 ( +8 ) /* failure for unknown reason       */

#if   ( CONF( HASH, CID ) == 0 ) /* SHA-1   */
#define HASH_CTX  SHA_CTX
#define HASH_LEN  SHA_DIGEST_LENGTH
#define HASH_INIT SHA1_Init
#define HASH_STEP SHA1_Update
#define HASH_FINI SHA1_Final
#define HASH      SHA1
#elif ( CONF( HASH, CID ) == 1 ) /* SHA-256 */
#define HASH_CTX  SHA256_CTX
#define HASH_LEN  SHA256_DIGEST_LENGTH
#define HASH_INIT SHA256_Init
#define HASH_STEP SHA256_Update
#define HASH_FINI SHA256_Final
#define HASH      SHA256
#elif ( CONF( HASH, CID ) == 2 ) /* SHA-384 */
#define HASH_CTX  SHA384_CTX
#define HASH_LEN  SHA384_DIGEST_LENGTH
#define HASH_INIT SHA384_Init
#define HASH_STEP SHA384_Update
#define HASH_FINI SHA384_Final
#define HASH      SHA384
#elif ( CONF( HASH, CID ) == 3 ) /* SHA-512 */
#define HASH_CTX  SHA512_CTX
#define HASH_LEN  SHA512_DIGEST_LENGTH
#define HASH_INIT SHA512_Init
#define HASH_STEP SHA512_Update
#define HASH_FINI SHA512_Final
#define HASH      SHA512
#endif

#endif

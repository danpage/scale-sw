/* Copyright (C) 2017 Daniel Page <dan@phoo.org>
 *
 * Use of this source code is restricted per the CC BY-SA license, a copy of
 * which can be found via http://creativecommons.org (and should be included 
 * as LICENSE.txt within the associated archive or repository).
 */

#include "target.h"

#define N_REGS 16
#define P_ADR 0
#define Q_ADR 1
#define N_ADR 2
#define D_ADR 3
#define M_ADR 4
#define ONE_ADR 5
#define COMP_ADR_1 6
#define A_ADR 7
#define B_ADR 8
#define X_ADR 9
#define Y_ADR 10
#define C_ADR 11
#define POW_ADR 12

#define l 1024

unsigned long copro_clock;
unsigned long f;
mpz_t R[N_REGS];

/*
All of these are pretty much identical to
those found in task2_coprocessor.py
*/

void copro_init() {
  copro_clock = 0;
  for (int i=0; i<N_REGS; ++i) {
    mpz_init(R[i]);
  }
}

void copro_reset() {
  copro_clock = 0;
  for (int i=0; i<N_REGS; ++i) {
    mpz_set_ui(R[i], 0);
  }
}

void copro_completeCycle(int reg) {
  copro_clock++;
  if (copro_clock == f) {
    // flip a random bit
    mpz_t flip_mask;
    mpz_init(flip_mask);
    mpz_setbit(flip_mask, rand()%l);
    mpz_xor(R[reg], R[reg], flip_mask);
    mpz_clear(flip_mask);
  }
}

void copro_add(int x, int y, int z, int N) {
  mpz_add(R[x], R[y], R[z]);
  mpz_mod(R[x], R[x], R[N]);
  copro_completeCycle(x);
}

void copro_sub(int x, int y, int z, int N) {
  mpz_sub(R[x], R[y], R[z]);
  mpz_mod(R[x], R[x], R[N]);
  copro_completeCycle(x);
}

void copro_mul(int x, int y, int z, int N) {
  mpz_mul(R[x], R[y], R[z]);
  mpz_mod(R[x], R[x], R[N]);
  copro_completeCycle(x);
}

void copro_mul_inverse(int x, int y, int N) {
  mpz_t gcd, dummy;
  mpz_init(gcd);
  mpz_init(dummy);
  mpz_gcdext(gcd, R[x], dummy, R[y], R[N]);
  if (mpz_cmp_ui(gcd, 1) == 0) { // gcd == 1
    mpz_mod(R[x], R[x], R[N]);
  } else {
    mpz_set_ui(R[x], 0);
  }

  mpz_clear(dummy);
  mpz_clear(gcd);
  copro_completeCycle(x);
}

void copro_add_inverse(int x, int y, int N) {
  mpz_mod(R[y], R[y], R[N]);
  mpz_sub(R[x], R[N], R[y]);
  copro_completeCycle(x);
}

void copro_copy_mod(int x, int y, int N) {
  mpz_mod(R[x], R[y], R[N]);
  copro_completeCycle(x);
}

void copro_copy(int x, int y) {
  mpz_set(R[x], R[y]);
  copro_completeCycle(x);
}

void copro_load_immediate(int x, const mpz_t val) {
  mpz_set(R[x], val);
  copro_completeCycle(x);
}

void copro_print_adr(int x) {
  mpz_out_str(stdout, 10, R[x]);
  printf("\n");
}

void pow_on_copro(int x, int y, int z, int N) {
  // uses left to right binary exp again
  copro_copy(POW_ADR, ONE_ADR);
  for (int i=1024; i>=0; --i) { // loop thru bits
    copro_mul(POW_ADR, POW_ADR, POW_ADR, N);
    if (mpz_tstbit(R[z], i) == 1) {
      copro_mul(POW_ADR, POW_ADR, y, N);
    }
  }
  copro_copy(x, POW_ADR);
}

int sign( mpz_t sigma, const mpz_t N, const mpz_t p, const mpz_t q, const mpz_t d, const mpz_t m, int delta ) {
  // "realistic" version

  // initialisation and loading args into 'coprocessor'
  f = delta;

  copro_reset();

  copro_load_immediate(P_ADR, p);
  copro_load_immediate(Q_ADR, q);
  copro_load_immediate(N_ADR, N);
  copro_load_immediate(D_ADR, d);
  copro_load_immediate(M_ADR, m);

  mpz_t one;
  mpz_init_set_ui(one, 1);
  copro_load_immediate(ONE_ADR, one);
  mpz_clear(one);

  // compute a = m^(d mod p-1) (mod p)
  copro_sub(COMP_ADR_1, P_ADR, ONE_ADR, P_ADR);
  copro_copy_mod(COMP_ADR_1, D_ADR, COMP_ADR_1);
  pow_on_copro(A_ADR, M_ADR, COMP_ADR_1, P_ADR);
  // similar for b = m^(d mod q-1) (mod q)
  copro_sub(COMP_ADR_1, Q_ADR, ONE_ADR, Q_ADR);
  copro_copy_mod(COMP_ADR_1, D_ADR, COMP_ADR_1);
  pow_on_copro(B_ADR, M_ADR, COMP_ADR_1, Q_ADR);

  // now compute x=1/p mod q, y=1/q mod p
  copro_mul_inverse(X_ADR, P_ADR, Q_ADR);
  copro_mul_inverse(Y_ADR, Q_ADR, P_ADR);

  // so c = bxp + ayq (mod N)
  copro_mul(C_ADR, B_ADR, X_ADR, N_ADR);
  copro_mul(C_ADR, C_ADR, P_ADR, N_ADR);
  copro_mul(COMP_ADR_1, A_ADR, Y_ADR, N_ADR);
  copro_mul(COMP_ADR_1, COMP_ADR_1, Q_ADR, N_ADR);
  copro_add(C_ADR, C_ADR, COMP_ADR_1, N_ADR);
  mpz_set(sigma, R[C_ADR]);

  return copro_clock;
}

int main( int argc, char* argv[] ) {
  uint8_t __N[] = { USER( N_DATA, CID ) };
  uint8_t __p[] = { USER( P_DATA, CID ) };
  uint8_t __q[] = { USER( Q_DATA, CID ) };
  uint8_t __d[] = { USER( D_DATA, CID ) };

  mpz_t N, p, q, d, m, sigma;

  mpz_inits( N, p, q, d, m, sigma, NULL );
  copro_init();

  #if   CONF( TARGET_D, CID ) &&  CONF( OBFUSCATE )
  unmask( __N,    USER( N_SIZE, CID ), USER( N_MASK, CID ) ); 
  unmask( __p,    USER( P_SIZE, CID ), USER( P_MASK, CID ) ); 
  unmask( __q,    USER( Q_SIZE, CID ), USER( Q_MASK, CID ) ); 
  unmask( __d,    USER( D_SIZE, CID ), USER( D_MASK, CID ) ); 
  #elif CONF( TARGET_R, CID ) || !CONF( OBFUSCATE )
  memset( __N, 0, USER( N_SIZE, CID )                      );
  memset( __p, 0, USER( P_SIZE, CID )                      );
  memset( __q, 0, USER( Q_SIZE, CID )                      );
  memset( __d, 0, USER( D_SIZE, CID )                      );
  #endif

  while( true ) {
    int lambda, delta;

    // 1. consume input

    CONSUME( gmp_scanf(  "%d", &delta ), 1 );
    CONSUME( gmp_scanf( "%ZX",  m     ), 1 );

    #if   CONF( TARGET_D, CID )
    mpz_import( N, USER( N_SIZE, CID ), -1, SIZEOF( uint8_t ), -1, 0, __N );
    mpz_import( p, USER( P_SIZE, CID ), -1, SIZEOF( uint8_t ), -1, 0, __p );
    mpz_import( q, USER( Q_SIZE, CID ), -1, SIZEOF( uint8_t ), -1, 0, __q );
    mpz_import( d, USER( D_SIZE, CID ), -1, SIZEOF( uint8_t ), -1, 0, __d );
    #elif CONF( TARGET_R, CID )
    CONSUME( gmp_scanf( "%ZX",  N     ), 1 );
    CONSUME( gmp_scanf( "%ZX",  p     ), 1 );
    CONSUME( gmp_scanf( "%ZX",  q     ), 1 );
    CONSUME( gmp_scanf( "%ZX",  d     ), 1 );
    #endif

    // 2. execute operation

    lambda = sign( sigma, N, p, q, d, m, delta );

    #if CONF( DEBUG )
    fprintf( stderr, "delta = " ); gmp_fprintf( stderr,  "%d\n", delta  );

    fprintf( stderr, "N     = " ); gmp_fprintf( stderr, "%ZX\n", N      );
    fprintf( stderr, "p     = " ); gmp_fprintf( stderr, "%ZX\n", p      );
    fprintf( stderr, "q     = " ); gmp_fprintf( stderr, "%ZX\n", q      );
    fprintf( stderr, "d     = " ); gmp_fprintf( stderr, "%ZX\n", d      );

    fprintf( stderr, "m     = " ); gmp_fprintf( stderr, "%ZX\n", m      );
    fprintf( stderr, "sigma = " ); gmp_fprintf( stderr, "%ZX\n", sigma  );
    #endif

    // 3. produce output

                                   gmp_fprintf( stdout,  "%d\n", lambda );
                                   gmp_fprintf( stdout, "%ZX\n", sigma  );

    // 4. flush streams

    fflush( stdout );
    fflush( stderr );
  }

  mpz_clears( N, p, q, d, m, sigma, NULL );

  return 0;
}

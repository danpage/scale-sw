changecom(`//')

// Copyright (C) 2017 Daniel Page <dan@phoo.org>
//
// Use of this source code is restricted per the CC BY-SA license, a copy of
// which can be found via http://creativecommons.org (and should be included 
// as LICENSE.txt within the associated archive or repository).

define(`ALU1',`( i8051_core->alu_x = $2,                         i8051_alu( $1 ), i8051_core->alu_r_lo )')
define(`ALU2',`( i8051_core->alu_x = $2, i8051_core->alu_y = $3, i8051_alu( $1 ), i8051_core->alu_r_lo )')

define(`LD',`( i8051_core->mem_addr = $3,                            i8051_mem_$1_ld_$2(), i8051_core->mem_data )')
define(`ST',`( i8051_core->mem_addr = $3, i8051_core->mem_data = $4, i8051_mem_$1_st_$2()                       )')

define(`INC16',{
  i8051_core->alu_f_cy =  0; 
  i8051_core->alu_x    = $2; 
  i8051_core->alu_y    =  1;

  i8051_alu( I8051_ALU_ADDC );

  $2 = i8051_core->alu_r_lo;

  i8051_core->alu_x    = $1; 
  i8051_core->alu_y    =  0;

  i8051_alu( I8051_ALU_ADDC );

  $1 = i8051_core->alu_r_lo;
})

define(`INC16_COND',{
  i8051_core->alu_f_cy =  0; 
  i8051_core->alu_x    = $2; 
  i8051_core->alu_y    =  1;

  i8051_alu( I8051_ALU_ADDC );

  if( $3 ) {
    $2 = i8051_core->alu_r_lo;
  }

  i8051_core->alu_x    = $1; 
  i8051_core->alu_y    =  0;

  i8051_alu( I8051_ALU_ADDC );

  if( $3 ) {
    $1 = i8051_core->alu_r_lo;
  }
})

define(`ADD16',{
  i8051_core->alu_f_cy = 0; 
  i8051_core->alu_x    = lo( $1 ); 
  i8051_core->alu_y    = lo( $2 );

  i8051_alu( I8051_ALU_ADDC );

  uint8_t lo = i8051_core->alu_r_lo;

  i8051_core->alu_x    = hi( $1 ); 
  i8051_core->alu_y    = hi( $2 );

  i8051_alu( I8051_ALU_ADDC );

  uint8_t hi = i8051_core->alu_r_lo;

  $1 = cat( hi, lo );
})

define(`ADD16_COND',{
  i8051_core->alu_f_cy = 0; 
  i8051_core->alu_x    = lo( $1 ); 
  i8051_core->alu_y    = lo( $2 );

  i8051_alu( I8051_ALU_ADDC );

  uint8_t lo = i8051_core->alu_r_lo;

  i8051_core->alu_x    = hi( $1 ); 
  i8051_core->alu_y    = hi( $2 );

  i8051_alu( I8051_ALU_ADDC );

  uint8_t hi = i8051_core->alu_r_lo;

  if( $3 ) {
    $1 = cat( hi, lo );
  }
})

define(`INST',
  case $1 : {
    for( int i = 1; i < $2; i++ ) {
      i8051_core->ir[ i ] = LD( pram, dir, i8051_core->pc++ );
    }

    $4

    i8051_core->tsc += $3;

    break;
  }
)

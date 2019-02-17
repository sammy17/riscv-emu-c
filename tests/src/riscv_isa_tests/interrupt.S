# See LICENSE for license details.

#*****************************************************************************
# lrsr.S
#-----------------------------------------------------------------------------
#
# Test LR/SC instructions.
#

#define TEST_FUNC_NAME interrupt
#define TEST_FUNC_TXT "interrupt \t"
#define TEST_FUNC_RET isaTestEnd

#include "riscv_test.h"
#include "test_macros.h"

RVTEST_RV64U
RVTEST_CODE_BEGIN

#define MTIME_ADDR 0x800000
#define MTIMECMP_ADDR 0x800008

#define FIFO_ADDR_TX 0xe0001030

#define TIMER_INTERVAL 2000
#define SIE_MASK (1<<1)
#define MIE_MASK (1<<3)


#ifdef __MACHINE_MODE
	#undef SIE_MASK
	#define SIE_MASK MIE_MASK
	#define stvec mtvec
	#define status mstatus
	#undef MIP_STIP
	#define MIP_STIP MIP_MTIP
#endif

interrupt_init:   
		lui t0,%hi(interrupt_handler)
  		addi t0,t0,%lo(interrupt_handler)
  		csrw stvec, t0                     
        li a0, MIP_STIP            
        csrs mie, a0   
        li a0, SIE_MASK
        csrs mstatus, a0            
        li a1, MTIME_ADDR			
        ld a0, 0(a1)               
        addi a0, a0, TIMER_INTERVAL
        li a1, MTIMECMP_ADDR		
        sd a0, 0(a1)
        j endint 
  
endint:
	j endint 


interrupt_handler:               
        li a1, FIFO_ADDR_TX
        li a2, 'a'
        sb a2, 0(a1)	
        li a1, MTIME_ADDR			
        ld a0, 0(a1)               
        addi a0, a0, TIMER_INTERVAL
        li a1, MTIMECMP_ADDR		
        sd a0, 0(a1)          
        mret                       

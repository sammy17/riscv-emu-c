/*
 * start.S
 *
 *  Created on: Nov 4, 2017
 *      Author: ijaz
 */

#include "firmware.h"
#include "test_macros.h"
#include "riscv_test.h"

	.section .text.boot
	.global _start
	.global isaTest
	.global isaTest_32mi
	.global dhrystoneBenchmark
	.global coremarkBenchmark
	.global isaTestEnd
	.global interrupt_message

/* Main program
 **********************************/

 // #define stvec_handler mtvec_handler

_start:
	jal ra,init
	/* trap */
	ebreak

initReg:
	/* initialize all registers */
	li sp, STACK_POINTER
	li x3, GLOBAL_POINTER
	addi x4, zero, 0
	addi x5, zero, 0
	addi x6, zero, 0
	addi x7, zero, 0
	addi x8, zero, 0
	addi x9, zero, 0
	addi x10, zero, 0
	addi x11, zero, 0
	addi x12, zero, 0
	addi x13, zero, 0
	addi x14, zero, 0
	addi x15, zero, 0
	addi x16, zero, 0
	addi x17, zero, 0
	addi x18, zero, 0
	addi x19, zero, 0
	addi x20, zero, 0
	addi x21, zero, 0
	addi x22, zero, 0
	addi x23, zero, 0
	addi x24, zero, 0
	addi x25, zero, 0
	addi x26, zero, 0
	addi x27, zero, 0
	addi x28, zero, 0
	addi x29, zero, 0
	addi x30, zero, 0
	addi x31, zero, 0
	ret

isaTest:
	jal ra,initReg
#ifdef IRQ
	jal ra,interrupt_init
#endif
	jal ra,add

isaTestEnd:
	jal ra,initReg
	ret //jal ra,init

dhrystoneBenchmark:
	jal ra,risvDhrystoneStartMsg
	jal ra,initReg
	jal ra,DHRYSTONE
	jal ra,init

coremarkBenchmark:
	jal ra,risvCoremarkStartMsg
	jal ra,initReg
	jal ra,COREMARK
	jal ra,init
	ret


//#define IRQ

#ifdef IRQ

#define MTIME_ADDR 0x800000
#define MTIMECMP_ADDR 0x800008

#define FIFO_ADDR_TX 0xe0001030

#define TIMER_INTERVAL 1000
#define SIE_MASK (1<<1)
#define MIE_MASK (1<<3)

#define __MACHINE_MODE_IRQ


#ifdef __MACHINE_MODE_IRQ
	#undef SIE_MASK
	#define SIE_MASK MIE_MASK
	#define stvec mtvec
	#define status mstatus
	#undef MIP_STIP
	#define MIP_STIP MIP_MTIP
#endif

	.global interrupt_reset

interrupt_init:   
		lui t0,%hi(interrupt_handler)
  		addi t0,t0,%lo(interrupt_handler)
  		csrw stvec, t0                        
        li a0, SIE_MASK
        csrs mstatus, a0            
        li a1, MTIME_ADDR			
        ld a0, 0(a1)               
        addi a0, a0, TIMER_INTERVAL
        li a1, MTIMECMP_ADDR		
        sd a0, 0(a1)
        li a0, MIP_STIP            
        csrs mie, a0
        ret

interrupt_handler:  
		addi	sp,sp,256 //giving initial offset to sp so that ew dont get negative values due to errors in tests
		addi	sp,sp,-32
		sd	t4,8(sp)
		sd	t5,16(sp)
		sd	t6,24(sp)
		li t5, FIFO_ADDR_TX
		li t6, '\n'
        sb t6, 0(t5)
        li t6, 'a'
        sb t6, 0(t5)
        li t6, '\n'
        sb t6, 0(t5)
        li t5, MTIME_ADDR			
        ld t4, 0(t5)               
        addi t4, t4, TIMER_INTERVAL
        li t5, MTIMECMP_ADDR		
        sd t4, 0(t5)  
		ld	t4,8(sp)
		ld	t5,16(sp)
		ld	t6,24(sp)   
		addi	sp,sp,32   
		addi	sp,sp,-255  // resetting sp
        mret 
        //        li t4, SIE_MASK
        //csrs mstatus, t4  
#endif
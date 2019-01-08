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

/* Main program
 **********************************/
_start:
	jal ra,init
	/* trap */
	ebreak

initReg:
	/* initialize all registers */
	li x2, STACK_POINTER
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
	jal ra,risvISATestStartMsg
	jal ra,initReg
	jal ra,add

isaTestEnd:
	jal ra,initReg
	ret

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

trap_vector:                                                                           
        csrr t5, mcause;                                                
        li t6, CAUSE_USER_ECALL;                                        
        beq t5, t6, write_tohost;                                       
        li t6, CAUSE_SUPERVISOR_ECALL;                                  
        beq t5, t6, write_tohost;                                       
        li t6, CAUSE_MACHINE_ECALL;                                     
        beq t5, t6, write_tohost;                                                      
        la t5, mtvec_handler;                                           
        beqz t5, 1f;                                                    
        jr t5;                                                                                
  1:    csrr t5, mcause;                                                
        bgez t5, handle_exception;                                      
        INTERRUPT_HANDLER;                                              
handle_exception:    

  other_exception:                                                      
                   
  1:    ori TESTNUM, TESTNUM, 1337;                                     
  write_tohost:                                                         
        sw TESTNUM, tohost, t5;                                         
        j write_tohost;   

reset_vector:                                                           
        RISCV_MULTICORE_DISABLE;                                        
        INIT_SATP;                                                     
        INIT_PMP;                                                       
        DELEGATE_NO_TRAPS;                                              
        li TESTNUM, 0;                                                  
        la t0, trap_vector;                                             
        csrw mtvec, t0;                                                 
        CHECK_XLEN;                                                     

        la t0, stvec_handler;                                           
        beqz t0, 1f;                                                    
        csrw stvec, t0;                                                 
		li t0, (1 << CAUSE_LOAD_PAGE_FAULT) | (1 << CAUSE_STORE_PAGE_FAULT) | (1 << CAUSE_FETCH_PAGE_FAULT) | (1 << CAUSE_MISALIGNED_FETCH) | (1 << CAUSE_USER_ECALL) | (1 << CAUSE_BREAKPOINT);                                 
        csrw medeleg, t0;                                               
        csrr t1, medeleg;                                               
        bne t0, t1, other_exception; 

1:      csrwi mstatus, 0;                                               
        //init;                                                           
        EXTRA_INIT;                                                     
        EXTRA_INIT_TIMER;                                               
        la t0, 1f;                                                      
        csrw mepc, t0;                                                  
        csrr a0, mhartid;                                               
        mret;                                                           
1:
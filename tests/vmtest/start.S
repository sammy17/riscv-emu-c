#include "firmware.h"
#include "test_macros.h"
#include "riscv_test.h"

	.section .text.boot
	.global init
    .global COREMARK

/* Main program
 **********************************/

 // #define stvec_handler mtvec_handler

#define FIFO_ADDR_TX 0xe0001030

RVTEST_RV64U
RVTEST_CODE_BEGIN

#ifdef CMARK

starter:
    jal ra,COREMARK

#else

starter:
    li sp,0x10000
    addi	sp,sp,-32
	sd	t4,8(sp)
	sd	t5,16(sp)
	sd	t6,24(sp)
	li t5, FIFO_ADDR_TX
	li t6, '\n'
    sb t6, 0(t5)
    li t6, '-'
    sb t6, 0(t5)
    li t6, '-'
    sb t6, 0(t5)
    li t6, '-'
    sb t6, 0(t5)
    li t6, '-'
    sb t6, 0(t5)
    li t6, 'F'
    sb t6, 0(t5)
    li t6, 'I'
    sb t6, 0(t5)
    li t6, 'R'
    sb t6, 0(t5)
    li t6, 'E'
    sb t6, 0(t5)
    li t6, 'D'
    sb t6, 0(t5)
    li t6, '-'
    sb t6, 0(t5)
    li t6, '-'
    sb t6, 0(t5)
    li t6, '-'
    sb t6, 0(t5)
    li t6, '-'
    sb t6, 0(t5)
    li t6, '\n'
    sb t6, 0(t5) 
	ld	t4,8(sp)
	ld	t5,16(sp)
	ld	t6,24(sp)   
	addi	sp,sp,32   

#endif

end:
    j end
    
RVTEST_CODE_END
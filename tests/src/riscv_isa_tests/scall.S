# See LICENSE for license details.

#*****************************************************************************
# scall.S
#-----------------------------------------------------------------------------
#
# Test syscall trap.
#

#define __MACHINE_MODE 1

#define TEST_FUNC_NAME ecall
#define TEST_FUNC_TXT "ecall \t"
#define TEST_FUNC_RET ebreak

#include "riscv_test.h"
#include "test_macros.h"

RVTEST_RV64S
RVTEST_CODE_BEGIN

#ifdef __MACHINE_MODE
  #define sscratch mscratch
  #define sstatus mstatus
  #define scause mcause
  #define sepc mepc
  #define sret mret
  #define stvec_handler2 mtvec_handler2
  #undef SSTATUS_SPP
  #define SSTATUS_SPP MSTATUS_MPP
#endif

  li TESTNUM, 2

  # This is the expected trap code.
  li t1, CAUSE_MACHINE_ECALL


1:
  li t0, SSTATUS_SPP
  csrc sstatus, t0
  la t0, 1f
  csrw sepc, t0
  sret
1:

  li TESTNUM, 1
do_scall:
  scall
  j fail

  TEST_PASSFAIL

  .align 2
  .global stvec_handler2
stvec_handler2:
  csrr t0, scause
  bne t0, t1, fail
  la t2, do_scall
  csrr t0, sepc
  bne t0, t2, fail
  j pass

RVTEST_CODE_END

  .data
RVTEST_DATA_BEGIN

  TEST_DATA

RVTEST_DATA_END

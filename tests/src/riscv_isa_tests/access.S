# See LICENSE for license details.

#*****************************************************************************
# access.S
#-----------------------------------------------------------------------------
#
# Test access-exception behavior.
#

#define TEST_FUNC_NAME access
#define TEST_FUNC_TXT "access \t"
#define TEST_FUNC_RET interrupt

#include "riscv_test.h"
#include "test_macros.h"

#ifdef __MACHINE_MODE
  #define sscratch mscratch
  #define sstatus mstatus
  #define scause mcause
  #define sepc mepc
  #define sret mret
  #define stvec mtvec
  #define stvec_handler1 mtvec_handler1
#endif

RVTEST_RV64M
RVTEST_CODE_BEGIN

setup_mtvec5:
  lui t0,%hi(mtvec_handler5)
  addi t0,t0,%lo(mtvec_handler5)
  csrw stvec, t0;

  .align 2


  # Flipping just the MSB should result in an illegal address for RV64.
  la t2, fail
  li t0, 1 << (__riscv_xlen - 1)
  xor t0, t0, t2

  # jalr to an illegal address should commit (hence should write rd).
  # after the pc is set to rs1, an access exception should be raised.
  li TESTNUM, 2
  li t1, CAUSE_FETCH_ACCESS
  la t3, 1f
  li t2, 0
  jalr t2, t0
1:

  # A load to an illegal address should not commit.
  li TESTNUM, 3
  li t1, CAUSE_LOAD_ACCESS
  la t3, 1f
  mv t2, t3
  lb t2, (t0)
  j fail
1:

  j pass

  TEST_PASSFAIL

  .align 2
  .global mtvec_handler5
mtvec_handler5:
  li a0, 2
  beq TESTNUM, a0, 2f
  li a0, 3
  beq TESTNUM, a0, 2f
  j fail

2:
  bne t2, t3, fail

  csrr t2, scause
  bne t2, t1, fail

  csrw sepc, t3
  sret

RVTEST_CODE_END

  .data
RVTEST_DATA_BEGIN

  TEST_DATA

RVTEST_DATA_END

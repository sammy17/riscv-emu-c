################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_UPPER_SRCS += \
../src/riscv_isa_tests/rv64ui/add.S \
../src/riscv_isa_tests/rv64ui/addw.S \
../src/riscv_isa_tests/rv64ui/addi.S \
../src/riscv_isa_tests/rv64ui/addiw.S \
../src/riscv_isa_tests/rv64ui/and.S \
../src/riscv_isa_tests/rv64ui/andi.S \
../src/riscv_isa_tests/rv64ui/auipc.S \
../src/riscv_isa_tests/rv64ui/beq.S \
../src/riscv_isa_tests/rv64ui/bge.S \
../src/riscv_isa_tests/rv64ui/bgeu.S \
../src/riscv_isa_tests/rv64ui/blt.S \
../src/riscv_isa_tests/rv64ui/bltu.S \
../src/riscv_isa_tests/rv64ui/bne.S \
../src/riscv_isa_tests/rv64ui/div.S \
../src/riscv_isa_tests/rv64ui/divu.S \
../src/riscv_isa_tests/rv64ui/j.S \
../src/riscv_isa_tests/rv64ui/jal.S \
../src/riscv_isa_tests/rv64ui/jalr.S \
../src/riscv_isa_tests/rv64ui/lb.S \
../src/riscv_isa_tests/rv64ui/lbu.S \
../src/riscv_isa_tests/rv64ui/lh.S \
../src/riscv_isa_tests/rv64ui/lhu.S \
../src/riscv_isa_tests/rv64ui/lui.S \
../src/riscv_isa_tests/rv64ui/lw.S \
../src/riscv_isa_tests/rv64ui/lwu.S \
../src/riscv_isa_tests/rv64ui/ld.S \
../src/riscv_isa_tests/rv64ui/mul.S \
../src/riscv_isa_tests/rv64ui/mulh.S \
../src/riscv_isa_tests/rv64ui/mulhsu.S \
../src/riscv_isa_tests/rv64ui/mulhu.S \
../src/riscv_isa_tests/rv64ui/or.S \
../src/riscv_isa_tests/rv64ui/ori.S \
../src/riscv_isa_tests/rv64ui/rem.S \
../src/riscv_isa_tests/rv64ui/remu.S \
../src/riscv_isa_tests/rv64ui/sb.S \
../src/riscv_isa_tests/rv64ui/sh.S \
../src/riscv_isa_tests/rv64ui/sw.S \
../src/riscv_isa_tests/rv64ui/sd.S \
../src/riscv_isa_tests/rv64ui/sll.S \
../src/riscv_isa_tests/rv64ui/slli.S \
../src/riscv_isa_tests/rv64ui/slliw.S \
../src/riscv_isa_tests/rv64ui/sllw.S \
../src/riscv_isa_tests/rv64ui/slt.S \
../src/riscv_isa_tests/rv64ui/sltu.S \
../src/riscv_isa_tests/rv64ui/slti.S \
../src/riscv_isa_tests/rv64ui/sltiu.S \
../src/riscv_isa_tests/rv64ui/sra.S \
../src/riscv_isa_tests/rv64ui/sraw.S \
../src/riscv_isa_tests/rv64ui/srai.S \
../src/riscv_isa_tests/rv64ui/sraiw.S \
../src/riscv_isa_tests/rv64ui/srl.S \
../src/riscv_isa_tests/rv64ui/srlw.S \
../src/riscv_isa_tests/rv64ui/srli.S \
../src/riscv_isa_tests/rv64ui/srliw.S \
../src/riscv_isa_tests/rv64ui/sub.S \

../src/riscv_isa_tests/rv64ui/xor.S \
../src/riscv_isa_tests/rv64ui/xori.S \

OBJS += \
./src/riscv_isa_tests/add.o \
./src/riscv_isa_tests/addi.o \
./src/riscv_isa_tests/and.o \
./src/riscv_isa_tests/andi.o \
./src/riscv_isa_tests/auipc.o \
./src/riscv_isa_tests/beq.o \
./src/riscv_isa_tests/bge.o \
./src/riscv_isa_tests/bgeu.o \
./src/riscv_isa_tests/blt.o \
./src/riscv_isa_tests/bltu.o \
./src/riscv_isa_tests/bne.o \
./src/riscv_isa_tests/div.o \
./src/riscv_isa_tests/divu.o \
./src/riscv_isa_tests/j.o \
./src/riscv_isa_tests/jal.o \
./src/riscv_isa_tests/jalr.o \
./src/riscv_isa_tests/lb.o \
./src/riscv_isa_tests/lbu.o \
./src/riscv_isa_tests/lh.o \
./src/riscv_isa_tests/lhu.o \
./src/riscv_isa_tests/lui.o \
./src/riscv_isa_tests/lw.o \
./src/riscv_isa_tests/mul.o \
./src/riscv_isa_tests/mulh.o \
./src/riscv_isa_tests/mulhsu.o \
./src/riscv_isa_tests/mulhu.o \
./src/riscv_isa_tests/or.o \
./src/riscv_isa_tests/ori.o \
./src/riscv_isa_tests/rem.o \
./src/riscv_isa_tests/remu.o \
./src/riscv_isa_tests/sb.o \
./src/riscv_isa_tests/sh.o \
./src/riscv_isa_tests/sll.o \
./src/riscv_isa_tests/slli.o \
./src/riscv_isa_tests/slt.o \
./src/riscv_isa_tests/slti.o \
./src/riscv_isa_tests/sra.o \
./src/riscv_isa_tests/srai.o \
./src/riscv_isa_tests/srl.o \
./src/riscv_isa_tests/srli.o \
./src/riscv_isa_tests/sub.o \
./src/riscv_isa_tests/sw.o \
./src/riscv_isa_tests/xor.o \
./src/riscv_isa_tests/xori.o \

S_UPPER_DEPS += \
./src/riscv_isa_tests/add.d \
./src/riscv_isa_tests/addi.d \
./src/riscv_isa_tests/and.d \
./src/riscv_isa_tests/andi.d \
./src/riscv_isa_tests/auipc.d \
./src/riscv_isa_tests/beq.d \
./src/riscv_isa_tests/bge.d \
./src/riscv_isa_tests/bgeu.d \
./src/riscv_isa_tests/blt.d \
./src/riscv_isa_tests/bltu.d \
./src/riscv_isa_tests/bne.d \
./src/riscv_isa_tests/div.d \
./src/riscv_isa_tests/divu.d \
./src/riscv_isa_tests/j.d \
./src/riscv_isa_tests/jal.d \
./src/riscv_isa_tests/jalr.d \
./src/riscv_isa_tests/lb.d \
./src/riscv_isa_tests/lbu.d \
./src/riscv_isa_tests/lh.d \
./src/riscv_isa_tests/lhu.d \
./src/riscv_isa_tests/lui.d \
./src/riscv_isa_tests/lw.d \
./src/riscv_isa_tests/mul.d \
./src/riscv_isa_tests/mulh.d \
./src/riscv_isa_tests/mulhsu.d \
./src/riscv_isa_tests/mulhu.d \
./src/riscv_isa_tests/or.d \
./src/riscv_isa_tests/ori.d \
./src/riscv_isa_tests/rem.d \
./src/riscv_isa_tests/remu.d \
./src/riscv_isa_tests/sb.d \
./src/riscv_isa_tests/sh.d \
./src/riscv_isa_tests/sll.d \
./src/riscv_isa_tests/slli.d \
./src/riscv_isa_tests/slt.d \
./src/riscv_isa_tests/slti.d \
./src/riscv_isa_tests/sra.d \
./src/riscv_isa_tests/srai.d \
./src/riscv_isa_tests/srl.d \
./src/riscv_isa_tests/srli.d \
./src/riscv_isa_tests/sub.d \
./src/riscv_isa_tests/sw.d \
./src/riscv_isa_tests/xor.d \
./src/riscv_isa_tests/xori.d \

# Each subdirectory must supply rules for building sources it contributes
src/riscv_isa_tests/%.o: ../src/riscv_isa_tests/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: RISC-V GCC/Newlib Assembler'
	riscv64-unknown-elf-gcc -mabi=lp64 -march=rv64im -x assembler-with-cpp -Wall -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -Wa,-march=rv64im -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



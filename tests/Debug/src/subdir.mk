################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/init.c \
../src/stdlib.c 

S_UPPER_SRCS += \
../src/start.S 

OBJS += \
./src/init.o \
./src/start.o \
./src/stdlib.o 

S_UPPER_DEPS += \
./src/start.d 

C_DEPS += \
./src/init.d \
./src/stdlib.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: RISC-V GCC/Newlib C Compiler'
	riscv64-unknown-elf-gcc -mabi=lp64 -march=rv64ima -fPIC -DTIME -DUSE_MYSTDLIB -DRISCV -O2 -Wall -ffreestanding -nostdlib  -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<" -I /home/dean/Linux-ext/newenu/tests/src
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: RISC-V GCC/Newlib Assembler'
	riscv64-unknown-elf-gcc -mabi=lp64 -march=rv64ima -x assembler-with-cpp -fPIC -Wall -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -Wa,-march=rv64ima -o "$@" "$<" -I /home/dean/Linux-ext/newenu/tests/src
	@echo 'Finished building: $<'
	@echo ' '



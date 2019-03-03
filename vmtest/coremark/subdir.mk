C_SRCS += \
./coremark/core_list_join.c \
./coremark/core_main.c \
./coremark/core_matrix.c \
./coremark/core_portme.c \
./coremark/core_state.c \
./coremark/core_util.c 

OBJS += \
./coremark/core_list_join.o \
./coremark/core_main.o \
./coremark/core_matrix.o \
./coremark/core_portme.o \
./coremark/core_state.o \
./coremark/core_util.o 

C_DEPS += \
./coremark/core_list_join.d \
./coremark/core_main.d \
./coremark/core_matrix.d \
./coremark/core_portme.d \
./coremark/core_state.d \
./coremark/core_util.d 


coremark/%.o: ./coremark/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: RISC-V GCC/Newlib C Compiler'
	riscv64-unknown-elf-gcc -mabi=lp64 -march=rv64im -fPIC -DTIME -DUSE_MYSTDLIB -DRISCV -O2 -Wall -ffreestanding -nostdlib  -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '
RISCV_GCC_OPTS ?= -static -mcmodel=medany -fvisibility=hidden -nostdlib -nostartfiles -ffunction-sections -std=gnu99 -O2 


compil :
	riscv64-unknown-elf-gcc -c stdlib.c                   -mabi=lp64 -march=rv64im $(RISCV_GCC_OPTS) -I./coremark/
	riscv64-unknown-elf-gcc -c coremark/core_list_join.c  -mabi=lp64 -march=rv64im $(RISCV_GCC_OPTS) -I./coremark/
	riscv64-unknown-elf-gcc -c coremark/core_main.c       -mabi=lp64 -march=rv64im $(RISCV_GCC_OPTS) -I./coremark/
	riscv64-unknown-elf-gcc -c coremark/core_matrix.c     -mabi=lp64 -march=rv64im $(RISCV_GCC_OPTS) -I./coremark/
	riscv64-unknown-elf-gcc -c coremark/core_portme.c 	  -mabi=lp64 -march=rv64im $(RISCV_GCC_OPTS) -I./coremark/
	riscv64-unknown-elf-gcc -c coremark/core_state.c      -mabi=lp64 -march=rv64im $(RISCV_GCC_OPTS) -I./coremark/
	riscv64-unknown-elf-gcc -c coremark/core_util.c       -mabi=lp64 -march=rv64im $(RISCV_GCC_OPTS) -I./coremark/
	riscv64-unknown-elf-gcc *.o -o main -mabi=lp64 -march=rv64im $(RISCV_GCC_OPTS) -T"link.ld"

clean :
	rm -rf *.o main 

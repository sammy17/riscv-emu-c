
RISCV_GCC_OPTS ?= -static -mcmodel=medany -fvisibility=hidden -nostdlib -nostartfiles  -std=gnu99 -O2 
RISCV_GCC_OPTS1 ?= -nostdlib -nostartfiles   -O2 

coremark_compil :
	riscv64-unknown-elf-gcc -c stdlib.c                   -mabi=lp64 -march=rv64im $(RISCV_GCC_OPTS1) -I./coremark/ -fPIC
	riscv64-unknown-elf-gcc -c coremark/core_list_join.c  -mabi=lp64 -march=rv64im $(RISCV_GCC_OPTS1) -I./coremark/ -fPIC
	riscv64-unknown-elf-gcc -c coremark/core_main.c       -mabi=lp64 -march=rv64im $(RISCV_GCC_OPTS1) -I./coremark/ -fPIC
	riscv64-unknown-elf-gcc -c coremark/core_matrix.c     -mabi=lp64 -march=rv64im $(RISCV_GCC_OPTS1) -I./coremark/ -fPIC
	riscv64-unknown-elf-gcc -c coremark/core_portme.c 	  -mabi=lp64 -march=rv64im $(RISCV_GCC_OPTS1) -I./coremark/ -fPIC
	riscv64-unknown-elf-gcc -c coremark/core_state.c      -mabi=lp64 -march=rv64im $(RISCV_GCC_OPTS1) -I./coremark/ -fPIC
	riscv64-unknown-elf-gcc -c coremark/core_util.c       -mabi=lp64 -march=rv64im $(RISCV_GCC_OPTS1) -I./coremark/ -fPIC


comp_coremark : clean coremark_compil
	riscv64-unknown-elf-gcc -c start.S -mabi=lp64 -march=rv64im $(RISCV_GCC_OPTS) -DCMARK
	riscv64-unknown-elf-gcc -c entry.S -mabi=lp64 -march=rv64im $(RISCV_GCC_OPTS)
	riscv64-unknown-elf-gcc -c string.c -mabi=lp64 -march=rv64im $(RISCV_GCC_OPTS)
	riscv64-unknown-elf-gcc -c vm.c -mabi=lp64 -march=rv64im $(RISCV_GCC_OPTS)
	riscv64-unknown-elf-gcc *.o -o main -mabi=lp64 -march=rv64im $(RISCV_GCC_OPTS) -T"link.ld"



comp_simple :
	riscv64-unknown-elf-gcc -c start.S -mabi=lp64 -march=rv64im $(RISCV_GCC_OPTS)
	riscv64-unknown-elf-gcc -c entry.S -mabi=lp64 -march=rv64im $(RISCV_GCC_OPTS)
	riscv64-unknown-elf-gcc -c string.c -mabi=lp64 -march=rv64im $(RISCV_GCC_OPTS)
	riscv64-unknown-elf-gcc -c vm.c -mabi=lp64 -march=rv64im $(RISCV_GCC_OPTS)
	riscv64-unknown-elf-gcc start.o entry.o string.o vm.o -o main -mabi=lp64 -march=rv64im $(RISCV_GCC_OPTS) -T"link.ld" 

coremark_only: clean coremark_compil
	riscv64-unknown-elf-gcc *.o -o main -mabi=lp64 -march=rv64im $(RISCV_GCC_OPTS) -T"link.ld"

clean :
	rm -rf *.o main 

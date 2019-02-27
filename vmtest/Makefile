RISCV_GCC_OPTS ?= -static -mcmodel=medany -fvisibility=hidden -nostdlib -nostartfiles  -std=gnu99 -O2 
comp :
	riscv64-unknown-elf-gcc -c start.S -mabi=lp64 -march=rv64im $(RISCV_GCC_OPTS)
	riscv64-unknown-elf-gcc -c entry.S -mabi=lp64 -march=rv64im $(RISCV_GCC_OPTS)
	riscv64-unknown-elf-gcc -c string.c -mabi=lp64 -march=rv64im $(RISCV_GCC_OPTS)
	riscv64-unknown-elf-gcc -c vm.c -mabi=lp64 -march=rv64im $(RISCV_GCC_OPTS)
	riscv64-unknown-elf-gcc start.o entry.o string.o vm.o -o main -mabi=lp64 -march=rv64im $(RISCV_GCC_OPTS) -T"link.ld" 

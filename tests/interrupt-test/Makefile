RISCV_GCC_OPTS ?= -static -mcmodel=medany -fvisibility=hidden -nostdlib -nostartfiles  -std=gnu99 -O2 

comp:
	riscv64-unknown-linux-gnu-gcc -c init.S -mabi=lp64 -march=rv64im $(RISCV_GCC_OPTS) -ffunction-sections
	riscv64-unknown-linux-gnu-gcc -c main.c -mabi=lp64 -march=rv64im $(RISCV_GCC_OPTS) -ffunction-sections
	riscv64-unknown-linux-gnu-gcc init.o main.o -o main -mabi=lp64 -march=rv64im $(RISCV_GCC_OPTS) -T"link.ld" -ffunction-sections -fPIC
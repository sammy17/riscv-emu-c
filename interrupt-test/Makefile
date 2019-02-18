
comp:
	riscv64-unknown-elf-gcc -c init.S -mabi=lp64 -march=rv64im -nostartfiles -ffunction-sections
	riscv64-unknown-elf-gcc -c main.c -mabi=lp64 -march=rv64im -nostartfiles -ffunction-sections
	riscv64-unknown-elf-gcc init.o main.o -o main -mabi=lp64 -march=rv64im -nostartfiles -fPIC -T"link.ld" -ffunction-sections
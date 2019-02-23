comp :
	riscv64-unknown-elf-gcc -c start.S -mabi=lp64 -march=rv64im -nostartfiles -ffunction-sections
	riscv64-unknown-elf-gcc -c entry.S -mabi=lp64 -march=rv64im -nostartfiles -ffunction-sections
	riscv64-unknown-elf-gcc -c string.c -mabi=lp64 -march=rv64im -nostartfiles -ffunction-sections
	riscv64-unknown-elf-gcc -c vm.c -mabi=lp64 -march=rv64im -nostartfiles 
	riscv64-unknown-elf-gcc start.o entry.o string.o vm.o -o main -mabi=lp64 -march=rv64im -nostartfiles -fPIC -T"link.ld" 

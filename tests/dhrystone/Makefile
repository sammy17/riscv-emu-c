RISCV_GCC_OPTS ?= -static -mcmodel=medany -fvisibility=hidden -nostdlib -nostartfiles -ffunction-sections -std=gnu99 -O2 


compil :
	riscv64-unknown-elf-gcc -c stdlib.c                   -fPIC -DTIME -DUSE_MYSTDLIB -DRISCV -O2 -Wall -ffreestanding -mabi=lp64 -march=rv64im $(RISCV_GCC_OPTS) -I./
	riscv64-unknown-elf-gcc -c dhry_1.c  			  -fPIC -DTIME -DUSE_MYSTDLIB -DRISCV -O2 -Wall -ffreestanding -mabi=lp64 -march=rv64im $(RISCV_GCC_OPTS) -I./
	riscv64-unknown-elf-gcc -c dhry_2.c           -fPIC -DTIME -DUSE_MYSTDLIB -DRISCV -O2 -Wall -ffreestanding -mabi=lp64 -march=rv64im $(RISCV_GCC_OPTS) -I./
	riscv64-unknown-elf-gcc -c start.S           -fPIC -DTIME -DUSE_MYSTDLIB -DRISCV -O2 -Wall -ffreestanding -mabi=lp64 -march=rv64im $(RISCV_GCC_OPTS) -I./
	riscv64-unknown-elf-gcc *.o -o main -mabi=lp64 -march=rv64im $(RISCV_GCC_OPTS) -T"link.ld" -fPIC -DTIME -DUSE_MYSTDLIB -DRISCV -O2 -Wall -ffreestanding

clean :
	rm -rf *.o main 

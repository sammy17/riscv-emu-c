


int main(int argc, char** argv) 
{ 
	char a[100];

	a = "riscv64-unknown-elf-objdump -D ";

	strcat(a,argv[0]);

	strcat(a," -O verilog g.hex")

	printf("%s\n",a);

	//system('riscv64-unknown-elf-objcopy '+filename+' -O verilog g.hex');

  
    return 0; 
} 
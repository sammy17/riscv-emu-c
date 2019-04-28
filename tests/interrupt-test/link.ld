OUTPUT_ARCH( "riscv" )
 ENTRY( "_start" )

SECTIONS{

	/* text: test code section */
	. = 0x80000000;
	.text :{
		*(.text)
	} 
	
	/* data: ReadOnly data segment */
	.rodata :{
    	*(.rodata)
    } 

	/* data: Initialized data segment */
	.data :{
		*(.data)
	}

	/* data: Uninitialized data segment */
	.bss  :{
		*(.bss) 
	} 
	
}

OUTPUT_ARCH( "riscv" )
 ENTRY( "_start" )

SECTIONS{

	/* text: test code section */
	. = 0x00000000;
	.text :{
		*(.text.main)
		*(.text.interrupt_init)
		*(.text.interrupt_handler)
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

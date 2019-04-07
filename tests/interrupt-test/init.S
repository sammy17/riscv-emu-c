
#define IRQ

#ifdef IRQ

#define MTIME_ADDR 0x800000
#define MTIMECMP_ADDR 0x800008

#define FIFO_ADDR_TX 0xe0001030

#define TIMER_INTERVAL 10000
#define SIE_MASK (1<<1)
#define MIE_MASK (1<<3)

#define MIP_STIP (1<<5)
#define MIP_MTIP (1<<7)

#define __MACHINE_MODE_IRQ


#ifdef __MACHINE_MODE_IRQ
	#undef SIE_MASK
	#define SIE_MASK MIE_MASK
	#define stvec mtvec
	#define status mstatus
	#undef MIP_STIP
	#define MIP_STIP MIP_MTIP
#endif

	.global interrupt_init
	.global interrupt_handler

interrupt_init:   
		lui t0,%hi(interrupt_handler)
  		addi t0,t0,%lo(interrupt_handler)
  		csrw stvec, t0                        
        li a0, SIE_MASK
        csrs mstatus, a0            
        li a1, MTIME_ADDR			
        ld a0, 0(a1)      
        li t0 , TIMER_INTERVAL         
        add a0, a0, t0
        li a1, MTIMECMP_ADDR		
        sd a0, 0(a1)
        li a0, MIP_STIP            
        csrs mie, a0
        ret

interrupt_handler:  
		addi	sp,sp,-32
		sd	t4,8(sp)
		sd	t5,16(sp)
		sd	t6,24(sp)
		li t5, FIFO_ADDR_TX
		li t6, '\n'
        sb t6, 0(t5)
        li t6, '-'
        sb t6, 0(t5)
        li t6, '-'
        sb t6, 0(t5)
        li t6, '-'
        sb t6, 0(t5)
        li t6, '-'
        sb t6, 0(t5)
        li t6, 'F'
        sb t6, 0(t5)
        li t6, 'I'
        sb t6, 0(t5)
        li t6, 'R'
        sb t6, 0(t5)
        li t6, 'E'
        sb t6, 0(t5)
        li t6, 'D'
        sb t6, 0(t5)
        li t6, '-'
        sb t6, 0(t5)
        li t6, '-'
        sb t6, 0(t5)
        li t6, '-'
        sb t6, 0(t5)
        li t6, '-'
        sb t6, 0(t5)
        li t6, '\n'
        sb t6, 0(t5)
        li t5, MTIME_ADDR			
        ld t4, 0(t5) 
        li t5, TIMER_INTERVAL             
        add t4, t4, t5
        li t5, MTIMECMP_ADDR		
        sd t4, 0(t5)  
		ld	t4,8(sp)
		ld	t5,16(sp)
		ld	t6,24(sp)   
		addi	sp,sp,32   
        mret 

#endif
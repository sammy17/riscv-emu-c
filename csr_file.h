#include "emu.h"

#define MCAUSE			0x342 	
#define MTVAL			0x343 	
#define MSCRATCH		0x340 	
#define MEPC			0x341 	
#define MIP				0x344 
#define TDATA3			0x7A3 	
#define SIE				0x104 
#define HPMCOUNTER20	0xC14 	
#define HPMCOUNTER23	0xC17 	
#define UEPC			0x041 	
#define USCRATCH		0x040 	
#define UTVAL			0x043 	
#define UCAUSE			0x042 	
#define UIP				0x044 
#define SSTATUS			0x100 
#define SEDELEG			0x102 
#define SIDELEG			0x103 
#define PMPCFG1			0x3A1 
#define PMPCFG0			0x3A0 
#define PMPCFG3			0x3A3 
#define PMPCFG2			0x3A2 
#define SATP			0x180 	
#define HPMCOUNTER15	0xC0F 	
#define HPMCOUNTER13	0xC0D 	
#define HPMCOUNTER14	0xC0E 	
#define HPMCOUNTER11	0xC0B 	
#define HPMCOUNTER12	0xC0C 	
#define HPMCOUNTER10	0xC0A 	
#define MHPMEVENT8		0x328 	
#define HPMCOUNTER15H	0xC8F 	
#define HPMCOUNTER13H	0xC8D 	
#define HPMCOUNTER14H	0xC8E 	
#define HPMCOUNTER11H	0xC8B 	
#define HPMCOUNTER12H	0xC8C 	
#define HPMCOUNTER10H	0xC8A 	
#define HPMCOUNTER25H	0xC99 	
#define HPMCOUNTER24H	0xC98 	
#define HPMCOUNTER21H	0xC95 	
#define HPMCOUNTER20H	0xC94 	
#define HPMCOUNTER23H	0xC97 	
#define HPMCOUNTER22H	0xC96 	
#define HPMCOUNTER17H	0xC91 	
#define HPMCOUNTER16H	0xC90 	
#define HPMCOUNTER19H	0xC93 	
#define HPMCOUNTER18H	0xC92 	
#define HPMCOUNTER21	0xC15 	
#define STVEC			0x105 	
#define SCOUNTEREN		0x106 	
#define HPMCOUNTER22	0xC16 	
#define HPMCOUNTER17	0xC11 	
#define HPMCOUNTER16	0xC10 	
#define HPMCOUNTER19	0xC13 	
#define HPMCOUNTER18	0xC12 	
#define HPMCOUNTER25	0xC19 	
#define HPMCOUNTER24	0xC18 	
#define HPMCOUNTER30	0xC1E 	
#define HPMCOUNTER29	0xC1D 	
#define HPMCOUNTER31	0xC1F 	
#define HPMCOUNTER26	0xC1A 	
#define HPMCOUNTER28	0xC1C 	
#define HPMCOUNTER27	0xC1B 	
#define UTVEC			0x005 	
#define UIE				0x004 
#define TDATA1			0x7A1 	
#define FFLAGS			0x001 	
#define USTATUS			0x000 
#define FCSR			0x003 	
#define FRM				0x002 
#define HPMCOUNTER30H	0xC9E 	
#define HPMCOUNTER29H	0xC9D 	
#define HPMCOUNTER31H	0xC9F 	
#define HPMCOUNTER26H	0xC9A 	
#define HPMCOUNTER28H	0xC9C 	
#define HPMCOUNTER27H	0xC9B 	
#define HPMCOUNTER8H	0xC88 	
#define HPMCOUNTER9H	0xC89 	
#define HPMCOUNTER6H	0xC86 	
#define HPMCOUNTER7H	0xC87 	
#define HPMCOUNTER4H	0xC84 	
#define HPMCOUNTER5H	0xC85 	
#define INSTRETH		0xC82 	
#define HPMCOUNTER3H	0xC83 	
#define CYCLEH			0xC80 	
#define TIMEH			0xC81 	
#define HPMCOUNTER6		0xC06 
#define HPMCOUNTER7		0xC07 
#define HPMCOUNTER4		0xC04 
#define HPMCOUNTER5		0xC05 
#define INSTRET			0xC02 
#define HPMCOUNTER3		0xC03 
#define CYCLE			0xC00 	
#define TIME			0xC01 	
#define HPMCOUNTER8		0xC08 
#define HPMCOUNTER9		0xC09 
#define MCOUNTEREN		0x306 	
#define MIE				0x304 
#define MTVEC			0x305 	
#define MEDELEG			0x302 
#define MIDELEG			0x303 
#define MSTATUS			0x300 
#define MISA			0x301 	
#define PMPADDR8		0x3B8 	
#define PMPADDR9		0x3B9 	
#define PMPADDR4		0x3B4 	
#define PMPADDR5		0x3B5 	
#define PMPADDR6		0x3B6 	
#define PMPADDR7		0x3B7 	
#define PMPADDR0		0x3B0 	
#define PMPADDR1		0x3B1 	
#define PMPADDR2		0x3B2 	
#define PMPADDR3		0x3B3 	
#define TDATA2			0x7A2 	
#define MHPMEVENT9		0x329 	
#define MHPMEVENT28		0x33C 
#define MHPMEVENT27		0x33B 
#define MHPMEVENT26		0x33A 
#define MHPMEVENT31		0x33F 
#define MHPMEVENT30		0x33E 
#define MHPMEVENT29		0x33D 
#define MHPMEVENT19		0x333 
#define MHPMEVENT18		0x332 
#define MHPMEVENT17		0x331 
#define MHPMEVENT16		0x330 
#define MHPMEVENT23		0x337 
#define MHPMEVENT22		0x336 
#define MHPMEVENT21		0x335 
#define MHPMEVENT20		0x334 
#define MHPMEVENT25		0x339 
#define MHPMEVENT24		0x338 
#define PMPADDR13		0x3BD 	
#define PMPADDR14		0x3BE 	
#define PMPADDR15		0x3BF 	
#define PMPADDR10		0x3BA 	
#define PMPADDR11		0x3BB 	
#define PMPADDR12		0x3BC 	
#define DCSR			0x7B0 	
#define DPC				0x7B1 
#define DSCRATCH		0x7B2 	
#define TSELECT			0x7A0 
#define MHPMCOUNTER26H	0xB9A 	
#define MHPMCOUNTER28H	0xB9B 	
#define MHPMCOUNTER29H	0xB9D 	
#define MHPMCOUNTER30H	0xB9E 	
#define MHPMCOUNTER31H	0xB9F 	
#define MHPMEVENT4		0x324 	
#define MHPMEVENT5		0x325 	
#define MHPMEVENT6		0x326 	
#define MHPMEVENT7		0x327 	
#define MHPMEVENT3		0x323 	
#define MHARTID			0xF14 
#define MVENDORID		0xF11 	
#define MARCHID			0xF12 
#define MIMPID			0xF13 	
#define MHPMCOUNTER26	0xB1A 	
#define MHPMCOUNTER28	0xB1B 	
#define MHPMCOUNTER29	0xB1D 	
#define MHPMCOUNTER30	0xB1E 	
#define MHPMCOUNTER31	0xB1F 	
#define MHPMCOUNTER9	0xB09 	
#define MHPMCOUNTER8	0xB08 	
#define MHPMCOUNTER7	0xB07 	
#define MHPMCOUNTER6	0xB06 	
#define MHPMCOUNTER5	0xB05 	
#define MHPMCOUNTER4	0xB04 	
#define MHPMCOUNTER3	0xB03 	
#define MINSTRET		0xB02 	
#define MCYCLE			0xB00 	
#define MHPMCOUNTER7H	0xB87 	
#define MHPMCOUNTER6H	0xB86 	
#define MHPMCOUNTER5H	0xB85 	
#define MHPMCOUNTER4H	0xB84 	
#define MHPMCOUNTER3H	0xB83 	
#define MINSTRETH		0xB82 	
#define MCYCLEH			0xB80 
#define SSCRATCH		0x140 	
#define SEPC			0x141 	
#define SCAUSE			0x142 	
#define STVAL			0x143 	
#define SIP				0x144 
#define MHPMCOUNTER9H	0xB89 	
#define MHPMCOUNTER8H	0xB88 	
#define MHPMCOUNTER15H	0xB8F 	
#define MHPMCOUNTER14H	0xB8E 	
#define MHPMCOUNTER13H	0xB8D 	
#define MHPMCOUNTER12H	0xB8B 	
#define MHPMCOUNTER10H	0xB8A 	
#define MHPMEVENT13		0x32D 
#define MHPMEVENT14		0x32E 
#define MHPMCOUNTER15	0xB0F 	
#define MHPMCOUNTER14	0xB0E 	
#define MHPMCOUNTER13	0xB0D 	
#define MHPMCOUNTER12	0xB0B 	
#define MHPMCOUNTER10	0xB0A 	
#define MHPMCOUNTER24	0xB18 	
#define MHPMCOUNTER25	0xB19 	
#define MHPMCOUNTER16	0xB10 	
#define MHPMCOUNTER17	0xB11 	
#define MHPMCOUNTER18	0xB12 	
#define MHPMCOUNTER19	0xB13 	
#define MHPMCOUNTER20	0xB14 	
#define MHPMCOUNTER21	0xB15 	
#define MHPMCOUNTER22	0xB16 	
#define MHPMCOUNTER23	0xB17 	
#define MHPMCOUNTER16H	0xB90 	
#define MHPMCOUNTER17H	0xB91 	
#define MHPMCOUNTER18H	0xB92 	
#define MHPMCOUNTER19H	0xB93 	
#define MHPMCOUNTER20H	0xB94 	
#define MHPMCOUNTER21H	0xB95 	
#define MHPMCOUNTER22H	0xB96 	
#define MHPMCOUNTER23H	0xB97 	
#define MHPMCOUNTER24H	0xB98 	
#define MHPMCOUNTER25H	0xB99 	
#define MHPMEVENT15		0x32F 
#define MHPMEVENT10		0x32A 
#define MHPMEVENT11		0x32B 
#define MHPMEVENT12		0x32C 	



struct mstat{
    uint8_t uie, sie, mie, upie, spie, mpie, spp, mpp, fs, xs, mprv, sum, mxr, tvm, tw, tsr, uxl, sxl, sd;
    mstat() {
        uie = 0; sie = 0; mie = 0;
        upie = 0; spie = 0; mpie = 0; 
        spp = 0; mpp = 0b11; 
        fs = 0; xs = 0; mprv = 0; sum = 0; mxr = 0; tvm = 0; tw = 0; tsr = 0; uxl = 0; sxl = 0; sd = 0; 
    }
    uint_t read_reg(){
        return (((uint_t)sd<<63)+((uint_t)sxl<<34)+((uint_t)uxl<<32)+(tsr<<22)+(tw<<21)+(tvm<<20)+(mxr<<19)+(sum<<18)+(mprv<<17)+(xs<<15)+(fs<<13)+(mpp<<11)+(spp<<8)+(mpie<<7)+(spie<<5)+(upie<<4)+(mie<<3)+(sie<<1)+uie);
    }

    void write_reg(uint_t val){
        uie = (val & 0b1); sie = ((val>>1)& 0b1); mie = ((val>>3)& 0b1);
        upie= ((val>>5)& 0b1); spie= ((val>>6)& 0b1); mpie= ((val>>7)& 0b1); 
        spp= ((val>>8)& 0b1); mpp = 0b11;//((val>>11)& 0b11); hard wire to 11 for now
        fs= ((val>>13)& 0b11); xs= ((val>>15)& 0b11); mprv= ((val>>17)& 0b1); sum= ((val>>18)& 0b1); mxr= ((val>>19)& 0b1); tvm= ((val>>20)& 0b1); tw= ((val>>21)& 0b1); tsr= ((val>>22)& 0b1); uxl= ((val>>32)& 0b11); sxl= ((val>>34)& 0b11); sd= ((val>>63)& 0b1); 
    }
} mstatus;

uint_t mscratch=0;

uint_t medeleg = 0;
uint_t sedeleg = 0;

uint_t mideleg = 0;
uint_t sideleg = 0;

uint_t misa = 0b1000100000001;

uint_t mepc = 0;

struct mtvec_t{
    uint8_t mode;
    uint_t base;
    mtvec_t() {
    	mode = 0;
    	base = 0;
    }
    uint_t read_reg(){
        return (mode+(base<<2));
    }

    void write_reg(uint_t val){
    	mode = val & 0b11;
    	base = (val & (MASK64 - 0b11));    
    }
} mtvec;

struct mcause_t{
    uint8_t interrupt;
    uint_t ecode;
    mcause_t() {
    	interrupt = 0;
    	ecode = 0;
    }
    uint_t read_reg(){
        return ((((1llu<<63)-1) & ecode)+(interrupt<<63));
    }

    void write_reg(uint_t val){
    	ecode = val & ((1llu<<63)-1);
    	interrupt = (val>>63) & 0b1;    
    }
} mcause;

struct scause_t{
    uint8_t interrupt;
    uint_t ecode;
    scause_t() {
    	interrupt = 0;
    	ecode = 0;
    }
    uint_t read_reg(){
        return ((((1llu<<63)-1) & ecode)+(interrupt<<63));
    }

    void write_reg(uint_t val){
    	ecode = val & ((1llu<<63)-1);
    	interrupt = (val>>63) & 0b1;    
    }
} scause;

struct ucause_t{
    uint8_t interrupt;
    uint_t ecode;
    ucause_t() {
    	interrupt = 0;
    	ecode = 0;
    }
    uint_t read_reg(){
        return ((((1llu<<63)-1) & ecode)+(interrupt<<63));
    }

    void write_reg(uint_t val){
    	ecode = val & ((1llu<<63)-1);
    	interrupt = (val>>63) & 0b1;    
    }
} ucause;

uint_t csr_read(uint_t csr_addr){
    switch(csr_addr){
        case MSTATUS :
            return mstatus.read_reg();
            break;
        case MSCRATCH :
        	return mscratch;
        	break;
        case MISA :
        	return misa;
        	break;
        case MEPC :
        	return mepc;
        	break;
        case MTVEC :
        	return mtvec.read_reg();
        	break;
        case MCAUSE :
        	return mcause.read_reg();
        	break;
        case SCAUSE :
        	return scause.read_reg();
        	break;
        case UCAUSE :
        	return ucause.read_reg();
        	break;
        case MEDELEG :
        	return medeleg;
        	break;
        case SEDELEG :
        	return sedeleg;
        	break;
        case MIDELEG :
        	return mideleg;
        	break;
        case SIDELEG :
        	return sideleg;
        	break;
        default:
            cout << "CSR not implemented : " << hex << csr_addr << endl;
            break;
    }
}

void csr_write(uint_t csr_addr, uint_t val){
    switch(csr_addr){
        case MSTATUS :
            mstatus.write_reg(val);
            break;
        case MSCRATCH :
        	mscratch = val;
        	break;
        case MISA :
        	misa = val;
        	break;
        case MEPC :
        	mepc = val;
        	break;
        case MTVEC :
        	//cout << "mtvecccc"<<val<<endl;
            mtvec.write_reg(val);
            break;
        case MCAUSE :
            mcause.write_reg(val);
            break;
        case SCAUSE :
            scause.write_reg(val);
            break;
        case UCAUSE :
            ucause.write_reg(val);
            break;
        case MEDELEG :
        	medeleg = val;
        	break;
        case SEDELEG :
        	sedeleg = val;
        	break;
        case MIDELEG :
        	mideleg = val;
        	break;
        case SIDELEG :
        	sideleg = val;
        	break;
        default:
            cout << "CSR not implemented : " << hex <<csr_addr << endl;
            break;
    }
}


plevel_t trap_mode_select(uint_t cause, bool interrupt, plevel_t current_privilage){
	uint_t mtrap_deleg_reg = 0;
	uint_t strap_deleg_reg = 0;
	switch(current_privilage){
		case MMODE : 
			return MMODE;
			break;

		case SMODE :
			if (interrupt)
				mtrap_deleg_reg = mideleg;
			else 
				mtrap_deleg_reg = medeleg;

			if (((mtrap_deleg_reg>>cause) & 0b1 ) == 1)
				return SMODE;
			else 
				return MMODE;
			break;

		case UMODE :
			if (interrupt){
				mtrap_deleg_reg = mideleg;
				strap_deleg_reg = sideleg;
			}
			else {
				mtrap_deleg_reg = medeleg;
				strap_deleg_reg = sedeleg;
			}

			if (((mtrap_deleg_reg>>cause) & 0b1 ) == 1){     		//delegating to smode
            	if (((strap_deleg_reg>>cause) & 0b1 ) == 1)      //delegating to umode
                	return UMODE;
            	else
                	return SMODE;
            }
        	else
        		return MMODE;

        	break;
	}
}
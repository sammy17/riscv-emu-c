#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <bits/stdc++.h>
#include <math.h>
#include <chrono>
#include <thread>
#include <algorithm> 
#include <map>

typedef uint64_t uint_t;

std::map<uint_t,std::string> csr_file = {		
{ 0x342 , "mcause"	}, 
{ 0x343 , "mtval"	}, 
{ 0x340 , "mscratch"	}, 
{ 0x341 , "mepc"	}, 
{ 0x344 , "mip"	}, 
{ 0x7A3 , "tdata3"	}, 
{ 0x104 , "sie"	}, 
{ 0xC14 , "hpmcounter20"	}, 
{ 0xC17 , "hpmcounter23"	}, 
{ 0x041 , "uepc"	}, 
{ 0x040 , "uscratch"	}, 
{ 0x043 , "utval"	}, 
{ 0x042 , "ucause"	}, 
{ 0x044 , "uip"	}, 
{ 0x100 , "sstatus"	}, 
{ 0x102 , "sedeleg"	}, 
{ 0x103 , "sideleg"	}, 
{ 0x3A1 , "pmpcfg1"	}, 
{ 0x3A0 , "pmpcfg0"	}, 
{ 0x3A3 , "pmpcfg3"	}, 
{ 0x3A2 , "pmpcfg2"	}, 
{ 0x180 , "satp"	}, 
{ 0xC0F , "hpmcounter15"	}, 
{ 0xC0D , "hpmcounter13"	}, 
{ 0xC0E , "hpmcounter14"	}, 
{ 0xC0B , "hpmcounter11"	}, 
{ 0xC0C , "hpmcounter12"	}, 
{ 0xC0A , "hpmcounter10"	}, 
{ 0x328 , "mhpmevent8"	}, 
{ 0xC8F , "hpmcounter15h"	}, 
{ 0xC8D , "hpmcounter13h"	}, 
{ 0xC8E , "hpmcounter14h"	}, 
{ 0xC8B , "hpmcounter11h"	}, 
{ 0xC8C , "hpmcounter12h"	}, 
{ 0xC8A , "hpmcounter10h"	}, 
{ 0xC99 , "hpmcounter25h"	}, 
{ 0xC98 , "hpmcounter24h"	}, 
{ 0xC95 , "hpmcounter21h"	}, 
{ 0xC94 , "hpmcounter20h"	}, 
{ 0xC97 , "hpmcounter23h"	}, 
{ 0xC96 , "hpmcounter22h"	}, 
{ 0xC91 , "hpmcounter17h"	}, 
{ 0xC90 , "hpmcounter16h"	}, 
{ 0xC93 , "hpmcounter19h"	}, 
{ 0xC92 , "hpmcounter18h"	}, 
{ 0xC15 , "hpmcounter21"	}, 
{ 0x105 , "stvec"	}, 
{ 0x106 , "scounteren"	}, 
{ 0xC16 , "hpmcounter22"	}, 
{ 0xC11 , "hpmcounter17"	}, 
{ 0xC10 , "hpmcounter16"	}, 
{ 0xC13 , "hpmcounter19"	}, 
{ 0xC12 , "hpmcounter18"	}, 
{ 0xC19 , "hpmcounter25"	}, 
{ 0xC18 , "hpmcounter24"	}, 
{ 0xC1E , "hpmcounter30"	}, 
{ 0xC1D , "hpmcounter29"	}, 
{ 0xC1F , "hpmcounter31"	}, 
{ 0xC1A , "hpmcounter26"	}, 
{ 0xC1C , "hpmcounter28"	}, 
{ 0xC1B , "hpmcounter27"	}, 
{ 0x005 , "utvec"	}, 
{ 0x004 , "uie"	}, 
{ 0x7A1 , "tdata1"	}, 
{ 0x001 , "fflags"	}, 
{ 0x000 , "ustatus"	}, 
{ 0x003 , "fcsr"	}, 
{ 0x002 , "frm"	}, 
{ 0xC9E , "hpmcounter30h"	}, 
{ 0xC9D , "hpmcounter29h"	}, 
{ 0xC9F , "hpmcounter31h"	}, 
{ 0xC9A , "hpmcounter26h"	}, 
{ 0xC9C , "hpmcounter28h"	}, 
{ 0xC9B , "hpmcounter27h"	}, 
{ 0xC88 , "hpmcounter8h"	}, 
{ 0xC89 , "hpmcounter9h"	}, 
{ 0xC86 , "hpmcounter6h"	}, 
{ 0xC87 , "hpmcounter7h"	}, 
{ 0xC84 , "hpmcounter4h"	}, 
{ 0xC85 , "hpmcounter5h"	}, 
{ 0xC82 , "instreth"	}, 
{ 0xC83 , "hpmcounter3h"	}, 
{ 0xC80 , "cycleh"	}, 
{ 0xC81 , "timeh"	}, 
{ 0xC06 , "hpmcounter6"	}, 
{ 0xC07 , "hpmcounter7"	}, 
{ 0xC04 , "hpmcounter4"	}, 
{ 0xC05 , "hpmcounter5"	}, 
{ 0xC02 , "instret"	}, 
{ 0xC03 , "hpmcounter3"	}, 
{ 0xC00 , "cycle"	}, 
{ 0xC01 , "time"	}, 
{ 0xC08 , "hpmcounter8"	}, 
{ 0xC09 , "hpmcounter9"	}, 
{ 0x306 , "mcounteren"	}, 
{ 0x304 , "mie"	}, 
{ 0x305 , "mtvec"	}, 
{ 0x302 , "medeleg"	}, 
{ 0x303 , "mideleg"	}, 
{ 0x300 , "mstatus"	}, 
{ 0x301 , "misa"	}, 
{ 0x3B8 , "pmpaddr8"	}, 
{ 0x3B9 , "pmpaddr9"	}, 
{ 0x3B4 , "pmpaddr4"	}, 
{ 0x3B5 , "pmpaddr5"	}, 
{ 0x3B6 , "pmpaddr6"	}, 
{ 0x3B7 , "pmpaddr7"	}, 
{ 0x3B0 , "pmpaddr0"	}, 
{ 0x3B1 , "pmpaddr1"	}, 
{ 0x3B2 , "pmpaddr2"	}, 
{ 0x3B3 , "pmpaddr3"	}, 
{ 0x7A2 , "tdata2"	}, 
{ 0x329 , "mhpmevent9"	}, 
{ 0x33C , "mhpmevent28"	}, 
{ 0x33B , "mhpmevent27"	}, 
{ 0x33A , "mhpmevent26"	}, 
{ 0x33F , "mhpmevent31"	}, 
{ 0x33E , "mhpmevent30"	}, 
{ 0x33D , "mhpmevent29"	}, 
{ 0x333 , "mhpmevent19"	}, 
{ 0x332 , "mhpmevent18"	}, 
{ 0x331 , "mhpmevent17"	}, 
{ 0x330 , "mhpmevent16"	}, 
{ 0x337 , "mhpmevent23"	}, 
{ 0x336 , "mhpmevent22"	}, 
{ 0x335 , "mhpmevent21"	}, 
{ 0x334 , "mhpmevent20"	}, 
{ 0x339 , "mhpmevent25"	}, 
{ 0x338 , "mhpmevent24"	}, 
{ 0x3BD , "pmpaddr13"	}, 
{ 0x3BE , "pmpaddr14"	}, 
{ 0x3BF , "pmpaddr15"	}, 
{ 0x3BA , "pmpaddr10"	}, 
{ 0x3BB , "pmpaddr11"	}, 
{ 0x3BC , "pmpaddr12"	}, 
{ 0x7B0 , "dcsr"	}, 
{ 0x7B1 , "dpc"	}, 
{ 0x7B2 , "dscratch"	}, 
{ 0x7A0 , "tselect"	}, 
{ 0xB9A , "mhpmcounter26h"	}, 
{ 0xB9B , "mhpmcounter28h"	}, 
{ 0xB9D , "mhpmcounter29h"	}, 
{ 0xB9E , "mhpmcounter30h"	}, 
{ 0xB9F , "mhpmcounter31h"	}, 
{ 0x324 , "mhpmevent4"	}, 
{ 0x325 , "mhpmevent5"	}, 
{ 0x326 , "mhpmevent6"	}, 
{ 0x327 , "mhpmevent7"	}, 
{ 0x323 , "mhpmevent3"	}, 
{ 0xF14 , "mhartid"	}, 
{ 0xF11 , "mvendorid"	}, 
{ 0xF12 , "marchid"	}, 
{ 0xF13 , "mimpid"	}, 
{ 0xB1A , "mhpmcounter26"	}, 
{ 0xB1B , "mhpmcounter28"	}, 
{ 0xB1D , "mhpmcounter29"	}, 
{ 0xB1E , "mhpmcounter30"	}, 
{ 0xB1F , "mhpmcounter31"	}, 
{ 0xB09 , "mhpmcounter9"	}, 
{ 0xB08 , "mhpmcounter8"	}, 
{ 0xB07 , "mhpmcounter7"	}, 
{ 0xB06 , "mhpmcounter6"	}, 
{ 0xB05 , "mhpmcounter5"	}, 
{ 0xB04 , "mhpmcounter4"	}, 
{ 0xB03 , "mhpmcounter3"	}, 
{ 0xB02 , "minstret"	}, 
{ 0xB00 , "mcycle"	}, 
{ 0xB87 , "mhpmcounter7h"	}, 
{ 0xB86 , "mhpmcounter6h"	}, 
{ 0xB85 , "mhpmcounter5h"	}, 
{ 0xB84 , "mhpmcounter4h"	}, 
{ 0xB83 , "mhpmcounter3h"	}, 
{ 0xB82 , "minstreth"	}, 
{ 0xB80 , "mcycleh"	}, 
{ 0x140 , "sscratch"	}, 
{ 0x141 , "sepc"	}, 
{ 0x142 , "scause"	}, 
{ 0x143 , "stval"	}, 
{ 0x144 , "sip"	}, 
{ 0xB89 , "mhpmcounter9h"	}, 
{ 0xB88 , "mhpmcounter8h"	}, 
{ 0xB8F , "mhpmcounter15h"	}, 
{ 0xB8E , "mhpmcounter14h"	}, 
{ 0xB8D , "mhpmcounter13h"	}, 
{ 0xB8B , "mhpmcounter12h"	}, 
{ 0xB8A , "mhpmcounter10h"	}, 
{ 0x32D , "mhpmevent13"	}, 
{ 0x32E , "mhpmevent14"	}, 
{ 0xB0F , "mhpmcounter15"	}, 
{ 0xB0E , "mhpmcounter14"	}, 
{ 0xB0D , "mhpmcounter13"	}, 
{ 0xB0B , "mhpmcounter12"	}, 
{ 0xB0A , "mhpmcounter10"	}, 
{ 0xB18 , "mhpmcounter24"	}, 
{ 0xB19 , "mhpmcounter25"	}, 
{ 0xB10 , "mhpmcounter16"	}, 
{ 0xB11 , "mhpmcounter17"	}, 
{ 0xB12 , "mhpmcounter18"	}, 
{ 0xB13 , "mhpmcounter19"	}, 
{ 0xB14 , "mhpmcounter20"	}, 
{ 0xB15 , "mhpmcounter21"	}, 
{ 0xB16 , "mhpmcounter22"	}, 
{ 0xB17 , "mhpmcounter23"	}, 
{ 0xB90 , "mhpmcounter16h"	}, 
{ 0xB91 , "mhpmcounter17h"	}, 
{ 0xB92 , "mhpmcounter18h"	}, 
{ 0xB93 , "mhpmcounter19h"	}, 
{ 0xB94 , "mhpmcounter20h"	}, 
{ 0xB95 , "mhpmcounter21h"	}, 
{ 0xB96 , "mhpmcounter22h"	}, 
{ 0xB97 , "mhpmcounter23h"	}, 
{ 0xB98 , "mhpmcounter24h"	}, 
{ 0xB99 , "mhpmcounter25h"	}, 
{ 0x32F , "mhpmevent15"	}, 
{ 0x32A , "mhpmevent10"	}, 
{ 0x32B , "mhpmevent11"	}, 
{ 0x32C , "mhpmevent12" }
};
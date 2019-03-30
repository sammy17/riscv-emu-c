#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <bits/stdc++.h>
#include <math.h>
#include <chrono>
#include <thread>
#include <algorithm> 
#include <map>

extern "C" {
#include "temu/temu.h"
#include "temu/cutils.h"
#include "temu/iomem.h"
#include "temu/virtio.h"
#include "temu/machine.h"
#ifdef CONFIG_FS_NET
#include "temu/fs_utils.h"
#include "temu/fs_wget.h"
#endif
#ifdef CONFIG_SLIRP
#include "temu/slirp/libslirp.h"
#endif
}

#include "csr_file.h"
#include "emu.h"

#include <stdarg.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <getopt.h>
#ifndef _WIN32
#include <termios.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/if_tun.h>
#endif
#include <sys/stat.h>
#include <signal.h>
using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds
using namespace std;

    
bool plic_served_irq = false; 
bool plic_pending_irq = false;

//#define DEBUG

template<class T>
T sign_extend(T x, const int bits) {
    T m = 1;
    m <<= bits - 1;
    return (x ^ m) - m;
}

template<class T>
T divi(T num1, T num2,int s) {
    if (num2==0){
        switch(s){
            case(0)  : return (T)(-1); break;
            case(1) : return (T)MASK64; break;
            case(2)  : return num1; break;
            case(3) : return num1; break;
        }
    } else if(num1==(-pow(2ull,63)) && num2==-1){
        if (s==0 || s==2){
            switch(s){
                case(0)  : return -pow(2ull,63); break;
                case(2)  : return 0; break;
            }
        }
    } else {
        ldiv_t div_result;
        switch(s){
            case(0)  :
                div_result = div((int64_t)num1,(int64_t)num2);
                return div_result.quot;
                break;
            case(1) :
                return num1/num2;
                break;
            case(2)  :
                div_result = div((int64_t)num1,(int64_t)num2);
                return div_result.rem;
                break;
            case(3) :
                return num1%num2;
                break;
        }
    }
}

template<class T>
T divi32(T num1, T num2,int s) {
    if (num2==0){
        switch(s){
            case(0)  : return (T)(-1); break;
            case(1) : return (T)MASK32; break;
            case(2)  : return num1; break;
            case(3) : return num1; break;
        }
    } else if(num1==(-pow(2,31)) && num2==-1){
        if (s==0 || s==2){
            switch(s){
                case(0)  : return -pow(2ull,31); break;
                case(2)  : return 0; break;
            }
        }
    } else {
        div_t div_result;
        switch(s){
            case(0)  :
                div_result = div((int32_t)num1,(int32_t)num2);
                return div_result.quot;
                break;
            case(1) :
                return num1/num2;
                break;
            case(2)  :
                div_result = div((int32_t)num1,(int32_t)num2);
                return div_result.rem;
                break;
            case(3) :
                return num1%num2;
                break;
        }
    }
}

int64_t signed_value(uint_t x){
  if (((x>>63) & 0b1) == 1)
      return (x ^ (1llu<<63)) - (1llu<<63);
  else
      return x;
}

int32_t signed_value32(uint_t x){
    uint32_t y = x & MASK32;
  if (((y>>31) & 0b1) == 1)
      return (y ^ (1lu<<31)) - (1lu<<31);
  else
      return y;
}

string reg_file_names [] = {"zero","ra","sp","gp","tp","t0","t1","t2","s0","s1","a0","a1","a2","a3","a4","a5","a6",\
                              "a7","s2","s3","s4","s5","s6","s7","s8","s9","s10","s11","t3","t4","t5","t6"};

void print_reg_file(vector<uint_t> reg_file){
    
    for (int i=0;i<32;i++){
        printf("%s : %lu\n",reg_file_names[i].c_str(),reg_file[i]);
    }
}

uint_t getINST(uint_t PC,vector<uint_t> * memory){
    if(PC%2==0)
        return ((MASK32) & (memory->at(PC/2)));
    else
        return ((memory->at(PC/2))>>32);
}

void early_stage_bootloader(){
    reg_file.at(10) = 0; //setting hart id to a0 = 0
    reg_file.at(11) = 0x00202000;
}


static uint_t clint_read( uint_t offset)
{

    uint_t val;

    switch(offset) {
    case 0xbff8:    //rtc time
        val = mtime;
        break;
    case 0x4000:    //timecmp
        val = mtimecmp;
        break;
    default:
        val = 0;
        break;
    }
    return val;
}

static void clint_write(uint_t offset, uint_t val)
{
    switch(offset) {
        case 0x4000:
            mtimecmp = val;
            break;
        default:
            break;
    }
}

static uint_t virtio_read( uint_t offset)
{
    return 0;
    uint_t val;

    switch(offset) {
    case 0xbff8:    //rtc time
        val = mtime;
        break;
    case 0x4000:    //timecmp
        val = mtimecmp;
        break;
    default:
        val = 0;
        break;
    }
    return val;
}

static void virtio_write(uint_t offset, uint_t val)
{


}

extern "C" uint32_t mem_read32(uint64_t addr){
    uint64_t load_data = memory.at((addr - DRAM_BASE)/8);
    uint64_t wb_data = 0;
    load_word(addr, load_data, wb_data);

    return (uint32_t)wb_data;
}

extern "C" void mem_write32(uint64_t addr, uint64_t data){
    uint64_t store_data = 0;
    store_data = memory.at((addr - DRAM_BASE)/8);
    uint64_t wb_data = 0;
    store_word(addr, store_data, data, wb_data);
    memory.at((addr - DRAM_BASE)/8) = ( 0xFFFFFFFF & wb_data);
}

extern "C" uint16_t mem_read16(uint64_t addr){
    uint64_t load_data = memory.at((addr - DRAM_BASE)/8);
    uint64_t wb_data = 0;
    load_halfw(addr, load_data, wb_data);
    return (uint16_t)wb_data;
}

extern "C" void mem_write16(uint64_t addr, uint64_t data){
    uint64_t store_data = memory.at((addr - DRAM_BASE)/8);
    uint64_t wb_data = 0;
    store_halfw(addr, store_data, data, wb_data);
    memory.at((addr - DRAM_BASE)/8) = ( 0xFFFFFFFF & wb_data);
}

extern "C" uint8_t mem_read8(uint64_t addr){
    uint64_t load_data = memory.at((addr - DRAM_BASE)/8);
    uint64_t wb_data = 0;
    load_byte(addr, load_data, wb_data);
    return (uint8_t)wb_data;
}

extern "C" void mem_write8(uint64_t addr, uint64_t data){
    uint64_t store_data = memory.at((addr - DRAM_BASE)/8);
    uint64_t wb_data = 0;
    store_byte(addr, store_data, data, wb_data);
    memory.at((addr - DRAM_BASE)/8) = ( 0xFF & wb_data);
}


/*
typedef enum {
    BF_MODE_RO,
    BF_MODE_RW,
    BF_MODE_SNAPSHOT,
} BlockDeviceModeEnum;
*/

BlockDevice drive1, *drive=&drive1;

char *fname;

//BlockDeviceModeEnum drive_mode;

VIRTIODevice *block_dev;//=&block_dev_sf;

#define PLIC_HART_BASE 0x200000
#define PLIC_HART_SIZE 0x1000
static uint_t plic_read( uint_t offset)
{
    
    uint_t val;

    switch(offset) {
        case PLIC_HART_BASE:
            val = 0;
            break;
        case PLIC_HART_BASE + 4:
         
            if (mip.SEIP==1){
                
                plic_served_irq = true;
                val =1;
                mip.SEIP = 0;
            }
            else 
                val = 0;
            break;
    }
    return val;
}

static void plic_write(uint_t offset, uint_t val)
{

    switch(offset) {
  
        case PLIC_HART_BASE + 4:
            plic_served_irq = false;
            if (plic_pending_irq)
                mip.SEIP = 1;
            else
                mip.SEIP = 0;
            break;
    }

}




void plic_set_irq_emu(int irq_num, int state)
{
    uint32_t mask;

    mask = 1 << (irq_num - 1);
    if (state){
        plic_pending_irq = true;
        if (~plic_served_irq)
            mip.SEIP = 1;
        else
            mip.SEIP = 0;
    }
    else{
        plic_pending_irq = false;
        mip.SEIP = 0;
    }
    //plic_update_mip(s);
}


int main(){

    /////////////////////////////// tinyemu init begin //////////////////////////////////
    VirtMachine *s;
    const char *path, *cmdline, *build_preload_file;
    int c, option_index, ii, ram_size, accel_enable;
    BOOL allow_ctrlc;
    BlockDeviceModeEnum drive_mode;
    VirtMachineParams p_s, *p = &p_s;

    ram_size = -1;
    allow_ctrlc = FALSE;
    (void)allow_ctrlc;
    drive_mode = BF_MODE_SNAPSHOT;
    accel_enable = -1;
    cmdline = NULL;
    build_preload_file = NULL;

    allow_ctrlc = TRUE;
    drive_mode = BF_MODE_RW;
    cmdline = "";    // append argument 
    ram_size = 512;


    //path = argv[optind++];
    virt_machine_set_defaults(p);
    virt_machine_load_config_file(p, "./temu/riscv64.cfg", NULL, NULL);   // loading from config file commented, default config used

    if (ram_size > 0) {
        p->ram_size = (uint64_t)ram_size << 20;
    }
    if (accel_enable != -1)
        p->accel_enable = accel_enable;
    if (cmdline) {
        vm_add_cmdline(p, cmdline);
    }

    /* open the files & devices */
    printf("Drive count : %d\n",(uint32_t)(p->drive_count));
    for(ii = 0; ii < p->drive_count; ii++) {

        BlockDevice *drive;
        char *fname;
        fname = get_file_path(p->cfg_filename, p->tab_drive[ii].filename);
        {
            drive = block_device_init(fname, drive_mode);
            printf("block device init\n");
            // emu_main();
            // exit(0);
        }
        free(fname);
        p->tab_drive[ii].block_dev = drive;
    }

    for(int iii = 0; iii < p->fs_count; iii++) {
        FSDevice *fs;
        const char *path;
        path = p->tab_fs[iii].filename;
        {
            char *fname;
            fname = get_file_path(p->cfg_filename, path);
            fs = fs_disk_init(fname);
            if (!fs) {
                fprintf(stderr, "%s: must be a directory\n", fname);
                exit(1);
            }
            free(fname);
        }
        p->tab_fs[iii].fs_dev = fs;
    }

    p->console = console_init(allow_ctrlc);

    p->rtc_real_time = TRUE;

    //s = virt_machine_init(p);
    //if (!s)
    //    exit(1);

    IRQSignal plic_irq;

    irq_init(&plic_irq, plic_set_irq_emu, 1);

    VIRTIOBusDef vbus_sf;
    VIRTIOBusDef *vbus= &vbus_sf;

    memset(vbus, 0, sizeof(*vbus));

    //for(i = 0; i < p->drive_count; i++) {
    vbus->irq = &plic_irq;
    block_dev = virtio_block_init(vbus, p->tab_drive[0].block_dev);

        //irq_num++;
        //s->virtio_count++;
    
    
    virt_machine_free_config(p);

    /////////////////////////////// tinyemu init end //////////////////////////////////
//    VIRTIOBusDef vbus_sf;
//    VIRTIOBusDef *vbus= &vbus_sf;
//    IRQSignal plic_irq;
//    //fname="/home/vithurson/buildroot-riscv-2018-10-20/output/images/rootfs.ext2";
//    fname="/home/dean/Linux-ext/buildroot/buildroot-2019.02/output/images/rootfs.ext2";
//    drive = block_device_init(fname, drive_mode);
//    ////////////////////////////////////////////////////////
//    vbus->addr = 0x40010000;
//    vbus->irq = &plic_irq;
//    block_dev=virtio_block_init(vbus, drive);
//    cout <<" value" <<hex<<virtio_mmio_read(block_dev,0 ,2 )<<endl;
    //exit(0);
//  exit(0);
    //////////////////////////////////////////////////////
    ifstream infile("data_hex.txt");
    string line;

    uint_t i = 0;

    uint_t PC = DRAM_BASE;
    uint_t PC_phy = 0;
    uint_t instruction = 0;
    bool branch = false;
    uint_t load_data = 0;
    uint_t store_data = 0;
    uint_t store_addr = 0;
    uint_t store_addr_phy = 0;
    uint_t load_addr = 0;
    uint_t load_addr_phy = 0;
    uint_t val = 0;
    uint_t itr = 0;

    uint_t cycle_count = 0;

    __uint128_t instr_count = 0;
    __uint128_t mult_temp=0;

    uint_t wb_data  = 0;
    uint_t ret_data = 0;
    uint_t lPC      = 0;
    uint_t rd       = 0;
    uint_t func3    = 0;
    uint_t rs1      = 0;
    uint_t rs2      = 0;
    uint_t func7    = 0;
    uint_t imm11_0  = 0;
    uint_t imm31_12 = 0;
    uint_t imm_j    = 0;
    uint_t imm_b    = 0;
    uint_t imm_s    = 0;
    uint_t csr_data = 0;
    plevel_t LR_cp;
    uint_t LR_count = 0;
    bool csr_bool = false;

    uint_t load_addr_phy_virt = 0;

    bool write_tval = false;

    plevel_t LR_cp64;
    uint_t LR_count64 = 0;

    uint_t amo_op   = 0;
    bool amo_reserve_valid = false;
    bool amo_reserve_valid64 = false;
    uint_t amo_reserve_addr = 0;
    uint_t amo_reserve_addr64 = 0;

    //initializing reg file
    //reg_file[2]  = 0x80040000 ; //SP
    //reg_file[11] = 0x80010000 ;

    bool ls_success = false;

    misa = 0b101000001000100000001;
    misa = (misa | (0b1llu<<63));

    enum opcode_t opcode;

    uint_t temp;

    while (std::getline(infile, line)) {            // Initialize memory with instructions
        temp = stoul(line,nullptr,16);
        if (i%2==0)
            memory.at(i/2) = temp;
        else
            memory.at(i/2) = ((memory.at(i/2)) | ((temp)<<32));
        i+=1;
    }
    #ifdef DEBUG
        printf("Mem load successful\n");
    #endif

    mstatus.fs = 1;

    uint_t testval_pre;
    uint_t testval_pos;

    memory.at(MTIME_ADDR/8) = 0;
    memory.at(MTIMECMP_ADDR/8) = -1;

    time_csr = 0;

    //early_stage_bootloader();

    //reg_file.at(10) = 0; //setting hart id to a0 = 0
    //reg_file.at(11) = 0x00202000;

    mhartid = 0;
    
    while (1){

        // cout << "a0 : "<<hex<<reg_file.at(11)<<endl;
        // if (PC == 0xffffffff80344b60){
        //    cout << "a0 : "<<hex<<reg_file.at(10)<<endl;
        //    cout << "a5 : "<<hex<<reg_file.at(15)<<endl;

        // }

        cycle_count += 1;

        cycle  = cycle_count ;
        instret  = cycle ;
        time_csr = cycle;
        mtime += 1;

        //#ifdef DEBUG
            //sleep_for(milliseconds(500));
        //cout << "PC : "<<PC<<endl;
        //#endif
        //sleep_for(milliseconds(10));

        //cout << "mstatus.mpp : "<<(uint_t)mstatus.mpp<<endl;

        //cout << "mstatus.mie : "<<(uint_t)mstatus.mie<<endl;

        //cout << "sp : "<<reg_file.at(2)<<endl;

        //cout << "PRIV : "<< (uint_t)cp<<endl;

        //cout << "a0 : "<<reg_file.at(10)<<endl;

        //cout << "t0 : "<<reg_file.at(5)<<endl;
        //cout << "t1 : "<<reg_file.at(6)<<endl;
        //cout << "t2 : "<<reg_file.at(7)<<endl;

        PC_phy = translate(PC, INST, cp);
            // cout << "PC : "<< hex << PC <<"PC_phy : "<<hex<<PC_phy<< endl;

        if (PC_phy==-1){
            //PC = excep_function(PC,CAUSE_FETCH_PAGE_FAULT,CAUSE_FETCH_PAGE_FAULT,CAUSE_FETCH_PAGE_FAULT,cp);
            //cout << "instruction fetch page fault PC: " <<hex<<PC<<endl;
            //INS_PAGE_FAULT = true;
            mtval = PC;
            PC = excep_function(PC+4,CAUSE_FETCH_PAGE_FAULT,CAUSE_FETCH_PAGE_FAULT,CAUSE_FETCH_PAGE_FAULT,cp);
            write_tval = false;
            switch(cp){
                case MMODE : 
                    mtval = mtval;
                    break;
                case SMODE :
                    stval = mtval;
                    mtval = 0;
                    break;
                case UMODE :
                    utval = mtval;
                    mtval = 0;
                    break;
            }
        continue;
            //continue; //exception will not occur if continue is there
        }

        //cout << "PC_phy  : "<< hex << PC_phy << endl;

        if (PC_phy >= DRAM_BASE){ // mapping to RAM
            PC_phy = PC_phy - DRAM_BASE; // mapping to emulator array memory
        }
        else{ // mapping to peripheral
            //cout << "peripheral access PC :"<< hex << PC_phy << endl;
            break;
        }
        //cout << "PC_phy converted : "<< hex << PC_phy << endl;

        instruction = getINST(PC_phy/4,&memory);


        reg_file[0] = 0;

        #ifdef DEBUG
            bitset<32> ins(instruction);
            cout << "Instruction : "<<ins << endl;
        #endif

        opcode = static_cast<opcode_t>((instruction) & 0b1111111);

        //cout << "mtimecmp : "<< mtimecmp<<endl;
        //cout << "mtime    : "<< mtime<<endl;

        wb_data = 0;

        rd      = ((instruction) >> 7 ) & 0b11111   ;
        func3   = ((instruction) >> 12) & 0b111     ;
        rs1     = ((instruction) >> 15) & 0b11111   ;
        rs2     = ((instruction) >> 20) & 0b11111   ;
        func7   = ((instruction) >> 25) & 0b1111111 ;

        imm11_0  = ((instruction) >> 20) & 0b111111111111 ;
        imm31_12 = ((instruction) >> 12) & 0xFFFFF ;       // extract 20 bits

        imm_j    = ((((instruction)>>31) & 0b1)<<20) + ((instruction) & (0b11111111<<12)) + ((((instruction)>>20) & 0b1)<<11) + ((((instruction)>>21) & 0b1111111111)<<1); //((instruction>>31) & 0b1)<<20 + (instruction & (0b11111111<<12)) + ((instruction>>20) & 0b1)<<11 +
        imm_b    = ((((instruction)>>31) & 0b1)<<12) + ((((instruction)>>7) & 0b1)<<11) + ((((instruction)>>25) & 0b111111)<<5) + (((instruction)>>7) & 0b11110) ;
        imm_s    = ((((instruction)>>25) & 0b1111111)<<5) + (((instruction)>>7) & 0b11111) ;

        amo_op   = ((instruction) >> 27) & 0b11111 ;


        if (amo_reserve_valid64){
            LR_count64 +=1;
            // if ( (opcode==jump) | (opcode==jumpr) | (opcode==cjump) | (opcode==load) | (opcode==store) | (opcode==fence) | (opcode==systm) ){
            //     cout << "Illegal instruction in between LR/SC 64 : " << (uint_t)opcode << endl;
            //     amo_reserve_valid64 = false;
            //     amo_reserve_addr64 = 0;
            //     LR_count64 = 0;
            // }
            // else if (LR_cp64 != cp){
            //     cout << "Privilege changed in between LR/SC 64 : " << endl;
            //     amo_reserve_valid64 = false;
            //     amo_reserve_addr64 = 0;
            //     LR_count64 = 0;
            // }
        }

        if (amo_reserve_valid){
            LR_count +=1;
        //     if ( (opcode==jump) | (opcode==jumpr) | (opcode==cjump) | (opcode==load) | (opcode==store) | (opcode==fence) | (opcode==systm) ){
        //         cout << "Illegal instruction in between LR/SC : " << (uint_t)opcode << endl;
        //         amo_reserve_valid = false;
        //         amo_reserve_addr = 0;
        //         LR_count = 0;
        //     }
        //     else if (LR_cp != cp){
        //         cout << "Privilege changed in between LR/SC : " << endl;
        //         amo_reserve_valid = false;
        //         amo_reserve_addr = 0;
        //         LR_count = 0;
        //     }
        }

        if ((PC%4)!=0){
            cout << "PC mis aligned "<<hex<<PC <<endl;
        }
        lPC = PC;
        PC += 4;
        switch(opcode){
            case lui :
                #ifdef DEBUG
                    printf("LUI\n");
                #endif
                //cout << "Instruction : "<<instruction << endl;
                wb_data = sign_extend<uint_t>((instruction & 0xFFFFF000),32);
                //cout << wb_data << endl;
                reg_file[rd] = wb_data;
                break;

            case auipc :
                #ifdef DEBUG
                    printf("AUIPC\n");
                #endif
                wb_data = (PC-4) + sign_extend<uint_t>(((imm31_12) << 12),32);
                reg_file[rd] = wb_data;
                //cout << "AUIPC : "<<hex<<wb_data<<endl;
                break;

            case jump :
                #ifdef DEBUG
                    cout << "JUMP - JAL" <<endl;
                #endif
                wb_data = PC;
                PC = (PC-4) + sign_extend<uint_t>(imm_j,21); //21 bits sign extend
                reg_file[rd] = wb_data ;// PC + 4 to rd
                break;

            case jumpr :
                #ifdef DEBUG
                    cout << "JUMPR - JALR" <<endl;
                #endif
                wb_data = PC;
                PC = (reg_file[rs1] + sign_extend<uint_t>(imm11_0,12)) & 0xFFFFFFFFFFFFFFFE; //setting LSB to 0 as spec page 20
                reg_file[rd] = wb_data;// PC + 4 to rd 
                break;

            case cjump :
                #ifdef DEBUG
                    printf("CJUMP\n");
                #endif
                switch(func3){
                    case 0b000 : 
                        //cout << "BEQ : "<<reg_file[rs1]<<" : "<<reg_file[rs2]<<endl;
                        branch = (reg_file[rs1] == reg_file[rs2]); 
                        break; //BEQ

                    case 0b001 : branch = (reg_file[rs1] != reg_file[rs2]); break; //BNE

                    case 0b100 : branch = (signed_value(reg_file[rs1]) < signed_value(reg_file[rs2])); break; //BLT

                    case 0b101 : branch = (signed_value(reg_file[rs1]) >= signed_value(reg_file[rs2])); break; //BGE

                    case 0b110 : branch = (reg_file[rs1] < reg_file[rs2]); break; //BLTU

                    case 0b111 : branch = (reg_file[rs1] >= reg_file[rs2]); break; //BGEU

                    default : printf("******INVALID INSTRUCTION******\nINS :%lu\nOPCODE :%lu\n",instruction,instruction & 0b1111111);
                        bitset<3> ins(func3);
                        cout<<  "func3 : "<<ins<<endl;
                        break;
                }
                if (branch==true)
                    PC = PC - 4 + sign_extend<uint_t>(imm_b,13);

                break;

            case load :
                #ifdef DEBUG
                    printf("LOAD\n");
                #endif
                load_addr = reg_file[rs1] + sign_extend<uint_t>(imm11_0,12);
                
                if ((load_addr != FIFO_ADDR_RX) && ((load_addr != FIFO_ADDR_TX))){
                    {
                        //cout << "load addr not translate : "<<hex<<load_addr<<endl;
                        load_addr_phy = translate(load_addr, LOAD, cp);

                        if (load_addr_phy==-1){
                            mtval = load_addr;
                            // LD_PAGE_FAULT = true;
                            
                                 //cout << "Page fault exception load : "<< hex << load_addr << "PC: " <<hex << PC<< " Physical PC : " <<hex <<PC_phy<< endl; 
                               PC = excep_function(PC,CAUSE_LOAD_PAGE_FAULT,CAUSE_LOAD_PAGE_FAULT,CAUSE_LOAD_PAGE_FAULT,cp);
                                switch(cp){
                                    case MMODE : 
                                    mtval = mtval;
                                    break;
                                    case SMODE :
                                    stval = mtval;
                                    mtval = 0;
                                    break;
                                    case UMODE :
                                    utval = mtval;
                                    mtval = 0;
                                    break;
                                }

                            continue;
                        }

                        if (load_addr_phy >= DRAM_BASE){ // mapping to RAM
                            load_addr_phy = load_addr_phy - DRAM_BASE; // mapping to emulator array memory

                            if (load_addr_phy > DRAM_SIZE){
                                cout << "ERROR : Exceeds RAM limit" << endl;
                                exit(0);
                            }
                        load_data = memory.at(load_addr_phy/8);

                        }
                        else{ // mapping to peripheral
                            cout << "peripheral access read"<< hex << load_addr_phy << endl;

                            if ((load_addr_phy >= CLINT_BASE) & (load_addr_phy <= (CLINT_BASE+CLINT_SIZE))){
                                load_data = clint_read(load_addr_phy-CLINT_BASE);
                            }
                            else if ((load_addr_phy >= PLIC_BASE) & (load_addr_phy <= (PLIC_BASE+PLIC_SIZE))){
                                load_data = ((uint_t)plic_read(load_addr_phy-PLIC_BASE))<<(32*((load_addr_phy%8)!=0));
                            }else if ((load_addr_phy >= VIRTIO_BASE) & (load_addr_phy <= (VIRTIO_BASE+VIRTIO_SIZE))){
                                load_addr_phy_virt = load_addr_phy/8;
                                load_addr_phy_virt = load_addr_phy_virt *8;
                                load_data =( (uint_t)virtio_mmio_read(block_dev,load_addr_phy_virt -VIRTIO_BASE+4,2)<<32)+ (virtio_mmio_read (block_dev,load_addr_phy_virt -VIRTIO_BASE,2));
                            }else {
                                cout << "New peripheral"<< hex << load_addr_phy<<endl;
                                exit(0);
                            }
                        }

                        //  if (load_addr_phy >= ((1llu)<<MEM_SIZE)){
                        //     //cout << "Physical memory limit          : "<<hex<<((1llu)<<MEM_SIZE)<<endl;
                        //     cout << "Physical memory limit exceeded : "<<hex<<load_addr_phy<<endl;
                        //     exit(0);
                        // }
                        switch(func3){
                            case 0b000 : 
                                if (!load_byte(load_addr_phy,load_data, wb_data)){
                                    LD_ADDR_MISSALIG = true;
                                    mtval = load_addr;                                  
                                } else {
                                    wb_data = sign_extend<uint_t>(wb_data & (0xFF)      , 8); 
                                    reg_file[rd] = wb_data;
                                }
                                break; //LB sign extend  8 bit value

                            case 0b001 : 
                                if (!load_halfw(load_addr_phy,load_data, wb_data)){
                                    LD_ADDR_MISSALIG = true;
                                    mtval = load_addr;
                                } else {
                                    wb_data = sign_extend<uint_t>(wb_data & (0xFFFF)    ,16); 
                                    reg_file[rd] = wb_data;
                                }
                                break; //LH sign extend 16 bit value

                            case 0b010 : 
                                if (!load_word(load_addr_phy,load_data, wb_data)){
                                    LD_ADDR_MISSALIG = true;
                                    mtval = load_addr;
                                    //cout << "LW"<<endl;
                                } else {
                                    wb_data = sign_extend<uint_t>(wb_data & (0xFFFFFFFF),32); 
                                    reg_file[rd] = wb_data;
                                }
                                break; //LW sign extend 32 bit value

                            case 0b100 : 
                                if (!load_byte(load_addr_phy,load_data, wb_data)){
                                    LD_ADDR_MISSALIG = true;
                                    mtval = load_addr;
                                    //cout << "LBU" <<hex<<load_addr_phy<<endl;
                                } else {
                                    wb_data = wb_data & 0xFF      ; 
                                    reg_file[rd] = wb_data;
                                }
                                break; //LBU zero extend  8 bit value

                            case 0b101 : 
                                if (!load_halfw(load_addr_phy,load_data, wb_data)){
                                    LD_ADDR_MISSALIG = true;
                                    mtval = load_addr;
                                    //cout << "LHU"<<endl;
                                } else {
                                    wb_data = wb_data & 0xFFFF    ; 
                                    reg_file[rd] = wb_data;
                                }
                                break; //LHU zero extend 16 bit value

                            case 0b110 : 
                                if (!load_word(load_addr_phy,load_data, wb_data)){
                                    LD_ADDR_MISSALIG = true;
                                    mtval = load_addr;
                                    //cout << "LWU"<<endl;
                                } else {
                                    wb_data = wb_data & 0xFFFFFFFF; 
                                    reg_file[rd] = wb_data;
                                }
                                break; //LWU zero extend 32 bit value

                            case 0b011 : 
                                if ((load_addr_phy%8)==0){
                                    wb_data = load_data ; 
                                    reg_file[rd] = wb_data;
                                    
                                } else {
                                    LD_ADDR_MISSALIG = true;
                                    mtval = load_addr;
                                    //cout << "LD"<<endl;
                                }
                                break;//LD

                            default : printf("******INVALID INSTRUCTION******\nINS :%lu\nOPCODE :%lu\n",instruction,instruction & 0b1111111);
                                bitset<3> ins(func3);
                                cout<<  "func3 : "<<ins<<endl;
                                break;
                        }
                    }
                } else if ( load_addr == FIFO_ADDR_RX ) {
                    
                    wb_data = 0 ;
                    reg_file[rd] = wb_data;
                }
                else if ( load_addr == FIFO_ADDR_TX ){
                    wb_data = 64;//(uint_t)getchar() ;
                    reg_file[rd] = wb_data;
                }
                break;

            case store :
                #ifdef DEBUG
                    printf("STORE\n");
                #endif
                store_addr = reg_file[rs1] + sign_extend<uint_t>(imm_s,12);
                
                if (store_addr != FIFO_ADDR_TX){                                 //& (store_addr != MTIME_ADDR) & (store_addr != MTIMECMP_ADDR)

                    store_addr_phy = translate(store_addr, STOR, cp);

                    if (store_addr_phy==-1){
                            //cout << "Page fault exception store"<<endl;
                            PC = excep_function(PC,CAUSE_STORE_PAGE_FAULT,CAUSE_STORE_PAGE_FAULT,CAUSE_STORE_PAGE_FAULT,cp);
                            // STORE_PAGE_FAULT = true;
                            mtval = store_addr;
                            switch(cp){
                                    case MMODE : 
                                        mtval = mtval;
                                        break;
                                    case SMODE :
                                        stval = mtval;
                                        mtval = 0;
                                        break;
                                    case UMODE :
                                        utval = mtval;
                                        mtval = 0;
                                        break;
                                }

                            continue;
                        }

                    if (store_addr_phy >= DRAM_BASE){ // mapping to RAM
                        store_addr_phy = store_addr_phy - DRAM_BASE; // mapping to emulator array memory
                          if (store_addr_phy >= ((1llu)<<MEM_SIZE)){
                                cout << "Physical memory limit exceeded : "<<hex<<store_addr_phy<<endl;
                                exit(0);
                            }else{
                                
                                store_data = memory.at(store_addr_phy/8);


                                switch(func3){                                                      // Setting lower n bits to 0 and adding storing value
                                    case 0b000 :
                                        val = reg_file[rs2] & 0xFF;
                                        ls_success = store_byte(store_addr_phy,store_data,val, wb_data);
                                        break;//SB  setting LSB 8 bit

                                    case 0b001 :
                                        val = reg_file[rs2] & 0xFFFF;
                                        ls_success = store_halfw(store_addr_phy,store_data,val, wb_data);
                                        break;//SH setting LSB 16 bit value

                                    case 0b010 :
                                        val = reg_file[rs2] & 0xFFFFFFFF;
                                        ls_success = store_word(store_addr_phy,store_data,val, wb_data);
                                        break;//SW setting LSB 32 bit value

                                    case 0b011 : 
                                        if ((store_addr_phy%8)==0){
                                            wb_data = reg_file[rs2] ; 
                                            ls_success = true;
                                        }else{
                                            ls_success = false;
                                        }
                                        break; //SD

                                    default : printf("******INVALID INSTRUCTION******\nINS :%lu\nOPCODE :%lu\n",instruction,instruction & 0b1111111);
                                    bitset<3> ins(func3);
                                    cout<<  "func3 : "<<func3<<endl;
                                    break;
                                }
                                if (!ls_success){
                                    //cout << "Mis-aligned store exception"<<endl;
                                    //PC = excep_function(PC,CAUSE_MISALIGNED_STORE,CAUSE_MISALIGNED_STORE,CAUSE_MISALIGNED_STORE,cp);
                                    STORE_ADDR_MISSALIG = true;
                                    mtval = store_addr;
                                }else {
                                    memory.at(store_addr_phy/8) = wb_data;
                                }
                            }
                    }
                    else{ // mapping to peripheral
                        // cout << "peripheral access write"<< hex << store_addr_phy << endl;
                        // cout << "mtime : " <<hex<<mtime<<endl;
                        // cout << "write value : "<<hex<<reg_file[rs2]<<endl;
                        // cout  << "offset : "<<hex<<store_addr_phy-CLINT_BASE<<endl;
                        // continue;
                        cout << "peripheral access write addr "<< hex << store_addr_phy << endl;
                        cout << "peripheral access write data "<< hex << reg_file[rs2] << endl;
                        if ((store_addr_phy >= CLINT_BASE) & (store_addr_phy <= (CLINT_BASE+CLINT_SIZE))){
                            clint_write(store_addr_phy-CLINT_BASE, reg_file[rs2]);
                        }
                        else if ((store_addr_phy >= PLIC_BASE) & (store_addr_phy <= (PLIC_BASE+PLIC_SIZE))){
                            plic_write(store_addr_phy-PLIC_BASE, reg_file[rs2]);
                        }else if ((store_addr_phy >= VIRTIO_BASE) & (store_addr_phy <= (VIRTIO_BASE+VIRTIO_SIZE))){
                            virtio_mmio_write(block_dev, store_addr_phy-VIRTIO_BASE, (reg_file[rs2] & 0xFFFFFFFF), 2);
                        }else {
                            cout << "New peripheral"<< hex << load_addr_phy<<endl;
                            exit(0);
                        }
                    }

                    /*if (store_addr >= ((1llu)<<MEM_SIZE)){ //memory access exception
                        cout << "Mem access exception : "<<hex<<store_addr<<endl;
                        mtval = store_addr;
                        PC = excep_function(PC,CAUSE_STORE_ACCESS,CAUSE_STORE_ACCESS,CAUSE_STORE_ACCESS,cp);   //access excep should be handled by translate function
                    }
                    else {*/
                    
                    
                  

                } 
                else if(store_addr == FIFO_ADDR_TX){
                    #ifdef DEBUG
                        printf("STORE2\n");
                    #endif
                    cout << (char)reg_file[rs2] ;
                }
                break;
            
            case iops  :
                #ifdef DEBUG
                    printf("IOPS %lu\n",imm11_0);
                #endif
                switch(func3){
                    case 0b000 :
                        wb_data = reg_file[rs1] + sign_extend<uint_t>(imm11_0,12); //ADDI
                        break;

                    case 0b010 :
                        wb_data = (signed_value(reg_file[rs1]) < signed_value(sign_extend<uint_t>(imm11_0,12))) ? 1 : 0; //SLTI
                        break;

                    case 0b011 :
                        wb_data = (reg_file[rs1] < sign_extend<uint_t>(imm11_0,12)) ? 1 : 0; //SLTIU
                        break;

                    case 0b111 :
                        wb_data = reg_file[rs1] & sign_extend<uint_t>(imm11_0,12); //ANDI
                        break;

                    case 0b110 :
                        wb_data = reg_file[rs1] | sign_extend<uint_t>(imm11_0,12); //ORI
                        break;

                    case 0b100 :
                        wb_data = reg_file[rs1] ^ sign_extend<uint_t>(imm11_0,12); //XORI
                        break;

                    case 0b001 :
                        wb_data = reg_file[rs1] << (imm11_0 & 0b111111); //SLLI
                        break;

                    case 0b101 :
                        if (((func7)>>1) == 0b000000)
                            wb_data = reg_file[rs1] >> ((imm11_0) & 0b111111); //SRLI
                        else if (((func7)>>1)== 0b010000){
                            wb_data = reg_file[rs1];
                            for (itr=0;itr<((imm11_0) & 0b111111);itr++){
                                wb_data = (wb_data & (1llu<<63)) | ((wb_data) >> (1)); //SRAI
                            }
                        }
                        break;
                    default :
                        printf("******INVALID INSTRUCTION******\nINS :%lu\nOPCODE :%lu\n",instruction,instruction & 0b1111111);
                        bitset<3> ins(func3);
                        cout<<  "func3 : "<<ins<<endl;
                        break;
                }
                reg_file[rd] = wb_data;
                break;

            case iops64 :
                #ifdef DEBUG
                    printf("IOPS64 %lu\n",imm11_0);
                #endif
                switch(func3){
                    case 0b000 :  //ADDIW
                        wb_data = sign_extend<uint_t>(MASK32 & (reg_file[rs1] + sign_extend<uint_t>(imm11_0,12)),32);
                        break;

                    case 0b001 : //SLLIW
                        wb_data = sign_extend<uint_t>(MASK32 & (reg_file[rs1] << (imm11_0 & 0b11111)),32);
                        break;

                    case 0b101 :
                        if (((func7)>>1) == 0b000000) //SRLIW
                            wb_data = sign_extend<uint_t>(MASK32 & ((reg_file[rs1] & MASK32) >> (imm11_0 & 0b11111)),32);
                        else if (((func7)>>1)== 0b010000){//SRAIW
                            wb_data = reg_file[rs1] & MASK32;
                            for (itr=0;itr<((imm11_0) & 0b11111);itr++){
                                wb_data = ((wb_data & ((1llu)<<31)) | ((wb_data) >> (1)));
                            }
                            wb_data = sign_extend<uint_t>(MASK32 & wb_data, 32);
                        }
                        break;
                }
                reg_file[rd] = wb_data;
                break;

            case rops :
                if (func7 == 0b0000001){
                    #ifdef DEBUG
                        printf("ROPS\n");
                    #endif
                    switch (func3) {
                        case 0b000 : //MUL
                            mult_temp = reg_file[rs1] * reg_file[rs2];
                            reg_file[rd] = (mult_temp) & MASK64;
                            break;

                        case 0b001 : //MULH
                            mult_temp = ((__uint128_t)signed_value(reg_file[rs1]) * (__uint128_t)signed_value(reg_file[rs2]));
                            reg_file[rd] = ((mult_temp) >> 64) & MASK64;
                            break;

                        case 0b010 : //MULHSU
                            mult_temp = ((__uint128_t)signed_value(reg_file[rs1]) * (__uint128_t)reg_file[rs2]);
                            reg_file[rd] = ((mult_temp) >> 64) & MASK64;
                            break;

                        case 0b011 : //MULHU
                            mult_temp = (__uint128_t)reg_file[rs1] * (__uint128_t)reg_file[rs2];
                            reg_file[rd] = ((mult_temp) >> 64) & MASK64;
                            break;

                        case 0b100 : //DIV
                            reg_file[rd] = (uint_t)divi<int64_t>(signed_value(reg_file[rs1]), signed_value(reg_file[rs2]),0);
                            //cout << hex << reg_file[rs1] <<endl;
                            //cout << hex << reg_file[rs2] <<endl;
                            //cout << divi<int64_t>(signed_value(reg_file[rs1]), signed_value(reg_file[rs2]),0) <<endl;
                            break;

                        case 0b101 : //DIVU
                            reg_file[rd] = divi<uint_t>(reg_file[rs1], reg_file[rs2],1);
                            break;

                        case 0b110 : //REM
                            reg_file[rd] = (uint_t)divi<int64_t>(signed_value(reg_file[rs1]), signed_value(reg_file[rs2]),2);
                            break;

                        case 0b111 : //REMU
                            reg_file[rd] = divi<uint_t>(reg_file[rs1], reg_file[rs2],3);
                            break;
                    }
                } else if (func7 == 0b0000000){
                    #ifdef DEBUG
                        printf("RROPS1\n");
                    #endif
                    switch(func3){
                        case 0b000 :
                            wb_data = reg_file[rs1] + reg_file[rs2]; //ADD
                            break;
                        case 0b010 :
                            wb_data = (signed_value(reg_file[rs1]) < signed_value(reg_file[rs2])) ? 1 : 0; //SLT
                            break;

                        case 0b011 :
                            wb_data = (reg_file[rs1] < reg_file[rs2]) ? 1 : 0; //SLTU
                            break;

                        case 0b111 :
                            wb_data = reg_file[rs1] & reg_file[rs2]; //AND
                            break;

                        case 0b110 :
                            wb_data = reg_file[rs1] | reg_file[rs2]; //OR
                            break;

                        case 0b100 :
                            wb_data = reg_file[rs1] ^ reg_file[rs2]; //XOR
                            break;

                        case 0b001 :
                            wb_data = ((reg_file[rs1]) << (reg_file[rs2] & 0b111111)); //SLL
                            break;

                        case 0b101 :
                            wb_data = reg_file[rs1] >> (reg_file[rs2] & 0b111111); //SRL
                            break;
                        default :
                            printf("******INVALID INSTRUCTION******\nINS :%lu\nOPCODE :%lu\n",instruction,instruction & 0b1111111);
                            bitset<3> ins(func3);
                            cout<<  "func3 : "<<ins<<endl;
                            break;

                    }
                    reg_file[rd] = wb_data;

                } else if (func7 == 0b0100000){
                    #ifdef DEBUG
                        printf("RROPS2\n");
                    #endif
                    switch(func3){
                        case 0b000 : //SUB
                            wb_data = reg_file[rs1] - reg_file[rs2];
                            break;

                        case 0b101 : //SRA
                            wb_data = reg_file[rs1];
                            for (itr=0;itr<(reg_file[rs2] & 0b111111);itr++){
                                wb_data = (( wb_data & ((1llu)<<63)) | ((wb_data) >> (1)));
                            }
                            break;
                    }
                    reg_file[rd] = wb_data;
                }
                break;

            case rops64 :
                if (func7 == 0b0000001){
                    #ifdef DEBUG
                        printf("ROPS64\n");
                    #endif
                    switch (func3) {
                        case 0b000 : //MULW
                            wb_data = sign_extend<uint_t>(((reg_file[rs1] & MASK32) * (reg_file[rs2] & MASK32)) & MASK32, 32) ;
                            break;
                        case 0b100 : //DIVW
                            wb_data = sign_extend<uint_t>(MASK32 & ((uint_t)divi32<int32_t>(signed_value32(reg_file[rs1] & MASK32), signed_value32(reg_file[rs2] & MASK32),0)),32);
                            break;

                        case 0b101 : //DIVUW
                            wb_data = sign_extend<uint_t>((uint_t)divi32<uint32_t>(reg_file[rs1], reg_file[rs2],1),32);
                            break;

                        case 0b110 : //REMW
                            wb_data = sign_extend<uint_t>(((uint_t)divi32<int32_t>(signed_value32(reg_file[rs1]), signed_value32(reg_file[rs2]),2) & MASK32 ),32);
                            break;

                        case 0b111 : //REMUW
                            wb_data = sign_extend<uint_t>((uint_t)divi32<uint32_t>(reg_file[rs1], reg_file[rs2],3),32);
                            break;
                    }
                    reg_file[rd] = wb_data;
                }
                else if (func7 == 0b0000000){
                    #ifdef DEBUG
                        printf("ROPS64-1\n");
                    #endif
                    switch(func3) {
                        case 0b000 : //ADDW
                            wb_data = sign_extend<uint_t>(((reg_file[rs1] + reg_file[rs2]) & MASK32), 32);
                            break;
                        case 0b001 : //SLLW
                            wb_data = sign_extend<uint_t>(((reg_file[rs1]) << ((reg_file[rs2]) & 0b11111)) & MASK32,32);
                            break;
                        case 0b101 : //SRLW
                            wb_data = sign_extend<uint_t>((reg_file[rs1] & MASK32) >> ((reg_file[rs2]) & 0b11111),32);
                            break;
                    }
                    reg_file[rd] = wb_data ;
                }
                else if (func7 == 0b0100000){
                    #ifdef DEBUG
                        printf("ROPS64-2\n");
                    #endif
                    switch(func3) {
                        case 0b000 : //SUBW
                            wb_data = sign_extend<uint_t>((((reg_file[rs1] & MASK32) - (reg_file[rs2] & MASK32)) & MASK32), 32);
                            break;
                        case 0b101 : //SRAW
                            wb_data = reg_file[rs1] & MASK32;
                            for (itr=0;itr<(reg_file[rs2] & 0b11111);itr++){
                                wb_data = ((wb_data & (1llu<<31)) | ((wb_data) >> (1)));
                            }
                            wb_data = sign_extend<uint_t>(wb_data, 32);
                            break;
                    }
                    reg_file[rd] = wb_data ;
                }
                break;

            case amo :
                if (func3 == 0b010) { //AMO.W-32
                    #ifdef DEBUG
                        printf("AMO 32\n");
                    #endif
                    load_addr = reg_file[rs1];
                    load_addr_phy = translate(load_addr, STOR, cp);

                    if (load_addr_phy==-1){
                            //cout << "Page fault exception store"<<endl;
                            PC = excep_function(PC,CAUSE_STORE_PAGE_FAULT,CAUSE_STORE_PAGE_FAULT,CAUSE_STORE_PAGE_FAULT,cp);
                            // STORE_PAGE_FAULT = true;
                            mtval = load_addr;
                            switch(cp){
                                    case MMODE : 
                                        mtval = mtval;
                                        break;
                                    case SMODE :
                                        stval = mtval;
                                        mtval = 0;
                                        break;
                                    case UMODE :
                                        utval = mtval;
                                        mtval = 0;
                                        break;
                                }

                            continue;
                        }
                    if(load_addr_phy>0x80000000) {
                        load_addr_phy = load_addr_phy -0x80000000;
                    }
                    else {
                        printf("illegal access %x %x\n",load_addr_phy,load_addr);
                        exit(0);
                    }
                    load_data = memory.at(load_addr_phy/8);
                    switch (amo_op){
                        case 0b00010 : //LR.W
                            
                            ls_success = load_word(load_addr_phy,load_data, wb_data);
                            if (!ls_success){
                                cout << "AMO-LR.W : Mis-aligned memory access" << endl;
                                STORE_ADDR_MISSALIG = true;
                                mtval = load_addr;
                            }
                            else {
                                ret_data = sign_extend<uint_t>(wb_data & MASK32,32);
                                amo_reserve_valid = true;
                                amo_reserve_addr = load_addr;
                                LR_count = 0;
                                LR_cp = cp;
                            }
                            break;

                        case 0b00011 : //SC.W
                            if (amo_reserve_valid && (reg_file[rs1]==amo_reserve_addr)){
                                store_data = reg_file[rs2] & MASK32;
                                

                                ls_success = store_word(load_addr_phy, load_data, store_data, wb_data);
                                if (!ls_success){
                                    cout << "AMO-SC.W : Mis-aligned memory access " << hex<< load_addr_phy%8<< endl;
                                    STORE_ADDR_MISSALIG = true;
                                    mtval = load_addr;
                                    ret_data = 1;
                                }
                                else {
                                    memory.at(load_addr_phy/8) = wb_data;
                                    ret_data = 0;
                                }
                            }
                            else {
                                ret_data = 1;
                            }
                            LR_count = 0;
                            amo_reserve_addr = 0;
                            amo_reserve_valid = false;
                            break;

                        case 0b00001 : //AMOSWAP.W
                            
                            ls_success = load_word(load_addr_phy,load_data, wb_data);
                            if (!ls_success){
                                cout << "AMO-SC.W : Mis-aligned memory access " << hex<< load_addr_phy%8<< endl;
                                mtval = load_addr;
                                STORE_ADDR_MISSALIG = true;
                            }
                            else {
                                ret_data = sign_extend<uint_t>((wb_data & MASK32),32);
                                wb_data = reg_file[rs2] & MASK32;
                                ls_success = store_word(load_addr,load_data,wb_data, store_data);
                                memory.at(load_addr_phy/8) = store_data;
                            }
                            break;

                        case 0b00000 : //AMOADD.W
                            
                            ls_success = load_word(load_addr_phy,load_data, wb_data);
                            if (!ls_success){
                                cout << "AMO-SC.W : Mis-aligned memory access " << hex<< load_addr_phy%8<< endl;
                                mtval = load_addr;
                                STORE_ADDR_MISSALIG = true;
                            }
                            else {
                                ret_data = sign_extend<uint_t>((wb_data & MASK32),32);
                                wb_data = ((wb_data & MASK32) + (reg_file[rs2] & MASK32)) & MASK32;
                                ls_success = store_word(load_addr,load_data,wb_data, store_data);
                                memory.at(load_addr_phy/8) = store_data;
                            }
                            break;

                        case 0b00100 : //AMOXOR.W
                            
                            ls_success = load_word(load_addr_phy,load_data, wb_data);
                            if (!ls_success){
                                cout << "AMO-SC.W : Mis-aligned memory access " << hex<< load_addr_phy%8<< endl;
                                mtval = load_addr;
                                STORE_ADDR_MISSALIG = true;
                            }
                            else {
                                ret_data = sign_extend<uint_t>((wb_data & MASK32),32);
                                wb_data = ((wb_data & MASK32) ^ (reg_file[rs2] & MASK32)) & MASK32;
                                ls_success = store_word(load_addr,load_data,wb_data, store_data);
                                memory.at(load_addr_phy/8) = store_data;
                            }
                            break;

                        case 0b01100 : //AMOAND.W
                            
                            ls_success = load_word(load_addr_phy,load_data, wb_data);
                            if (!ls_success){
                                cout << "AMO-SC.W : Mis-aligned memory access " << hex<< load_addr_phy%8<< endl;
                                mtval = load_addr;
                                STORE_ADDR_MISSALIG = true;
                            }
                            else {
                                ret_data = sign_extend<uint_t>((wb_data & MASK32),32);
                                wb_data = ((wb_data & MASK32) & (reg_file[rs2] & MASK32)) & MASK32;
                                ls_success = store_word(load_addr,load_data,wb_data, store_data);
                                memory.at(load_addr_phy/8) = store_data;
                            }
                            break;

                        case 0b01000 : //AMOOR.W
                            
                            ls_success = load_word(load_addr_phy,load_data, wb_data);
                            if (!ls_success){
                                cout << "AMO-SC.W : Mis-aligned memory access " << hex<< load_addr_phy%8<< endl;
                                mtval = load_addr;
                                STORE_ADDR_MISSALIG = true;
                            }
                            else {
                                ret_data = sign_extend<uint_t>((wb_data & MASK32),32);
                                wb_data = ((wb_data & MASK32) | (reg_file[rs2] & MASK32)) & MASK32;
                                ls_success = store_word(load_addr,load_data,wb_data, store_data);
                                memory.at(load_addr_phy/8) = store_data;
                            }
                            break;

                        case 0b10000 : //AMOMIN.W
                            
                            ls_success = load_word(load_addr_phy,load_data, wb_data);
                            if (!ls_success){
                                cout << "AMO-SC.W : Mis-aligned memory access " << hex<< load_addr_phy%8<< endl;
                                mtval = load_addr;
                                STORE_ADDR_MISSALIG = true;
                            }
                            else {
                                ret_data = sign_extend<uint_t>((wb_data & MASK32),32);
                                wb_data = min(signed_value32(wb_data & MASK32), signed_value32(reg_file[rs2] & MASK32)) & MASK32;
                                ls_success = store_word(load_addr,load_data,wb_data, store_data);
                                memory.at(load_addr_phy/8) = store_data;
                            }
                            break;

                        case 0b10100 : //AMOMAX.W
                            
                            ls_success = load_word(load_addr_phy,load_data, wb_data);
                            if (!ls_success){
                                cout << "AMO-SC.W : Mis-aligned memory access " << hex<< load_addr_phy%8<< endl;
                                mtval = load_addr;
                                STORE_ADDR_MISSALIG = true;
                            }
                            else {
                                ret_data = sign_extend<uint_t>((wb_data & MASK32),32);
                                wb_data = max(signed_value32(wb_data & MASK32), signed_value32(reg_file[rs2] & MASK32)) & MASK32;
                                ls_success = store_word(load_addr,load_data,wb_data, store_data);
                                memory.at(load_addr_phy/8) = store_data;
                            }
                            break;

                        case 0b11000 : //AMOMINU.W
                            
                            ls_success = load_word(load_addr_phy,load_data, wb_data);
                            if (!ls_success){
                                cout << "AMO-SC.W : Mis-aligned memory access " << hex<< load_addr_phy%8<< endl;
                                mtval = load_addr;
                                STORE_ADDR_MISSALIG = true;
                            }
                            else {
                                ret_data = sign_extend<uint_t>((wb_data & MASK32),32);
                                wb_data = min((wb_data & MASK32), (reg_file[rs2] & MASK32)) & MASK32;
                                ls_success = store_word(load_addr,load_data,wb_data, store_data);
                                memory.at(load_addr_phy/8) = store_data;
                            }
                            break;

                        case 0b11100 : //AMOMAXU.W
                            
                            ls_success = load_word(load_addr_phy,load_data, wb_data);
                            if (!ls_success){
                                cout << "AMO-SC.W : Mis-aligned memory access " << hex<< load_addr_phy%8<< endl;
                                mtval = load_addr_phy;
                                STORE_ADDR_MISSALIG = true;
                            }
                            else {
                                ret_data = sign_extend<uint_t>((wb_data & MASK32),32);
                                wb_data = max((wb_data & MASK32), (reg_file[rs2] & MASK32)) & MASK32;
                                ls_success = store_word(load_addr,load_data,wb_data, store_data);
                                memory.at(load_addr_phy/8) = store_data;
                            }
                            break;

                        default :
                            printf("******INVALID INSTRUCTION******\nINS :%lu\nOPCODE :%lu\n",instruction,(uint_t)opcode);
                            bitset<5> ins(amo_op);
                            cout<<  "amo op : "<<ins<<endl;
                            //PC = excep_function(PC,CAUSE_ILLEGAL_INSTRUCTION,CAUSE_ILLEGAL_INSTRUCTION,CAUSE_ILLEGAL_INSTRUCTION,cp);
                            mtval = instruction;
                            ILL_INS = true;
                            break;   
                    }
                    reg_file[rd] = ret_data;
                }
                else if (func3 == 0b011){
                    #ifdef DEBUG
                        printf("AMO 64\n");
                    #endif
                        load_addr = reg_file[rs1];
                        load_addr_phy = translate(load_addr, STOR, cp);

                        if (load_addr_phy==-1){
                            //cout << "Page fault exception store"<<endl;
                            PC = excep_function(PC,CAUSE_STORE_PAGE_FAULT,CAUSE_STORE_PAGE_FAULT,CAUSE_STORE_PAGE_FAULT,cp);
                            // STORE_PAGE_FAULT = true;
                            mtval = load_addr;
                            switch(cp){
                                    case MMODE : 
                                        mtval = mtval;
                                        break;
                                    case SMODE :
                                        stval = mtval;
                                        mtval = 0;
                                        break;
                                    case UMODE :
                                        utval = mtval;
                                        mtval = 0;
                                        break;
                                }

                            continue;
                        }
                    if(load_addr_phy>0x80000000) {
                        load_addr_phy = load_addr_phy -0x80000000;
                    }
                    else {
                       printf("illegal access %x %x\n",load_addr_phy,load_addr);
                        exit(0);
                    }

                    wb_data = memory.at(load_addr_phy/8);
                    switch (amo_op){ 
                        case 0b00010 : //LR.D
                            
                            if ((load_addr_phy%8)!=0){
                                cout << "AMO-LR.D : Mis-aligned memory access" << endl;
                                mtval = load_addr;
                                STORE_ADDR_MISSALIG = true;
                            }
                            else {
                                ret_data = wb_data;
                                amo_reserve_valid64 = true;
                                amo_reserve_addr64 = reg_file[rs1];
                                LR_count64 = 0;
                                LR_cp64 = cp;
                            }
                            break;

                        case 0b00011 : //SC.D
                            if (amo_reserve_valid64 && (reg_file[rs1]==amo_reserve_addr64)){
                                store_data = reg_file[rs2];
                                if ((load_addr_phy%8)!=0){
                                    cout << "AMO-SC.D : Mis-aligned memory access" << endl;
                                    mtval = load_addr;
                                    STORE_ADDR_MISSALIG = true;
                                }
                                else {
                                    memory.at(load_addr_phy/8) = store_data;
                                    ret_data = 0;
                                }
                            }
                            else {
                                ret_data = 1;
                            }
                            amo_reserve_addr64 = 0;
                            amo_reserve_valid64 = false;
                            break;

                        case 0b00001 : //AMOSWAP.D
                            
                            if ((load_addr_phy%8)!=0){
                                cout << "AMO-SC.W : Mis-aligned memory access " << hex<< load_addr_phy%8<< endl;
                                mtval = load_addr;
                                STORE_ADDR_MISSALIG = true;
                            }
                            else {
                                ret_data = wb_data;
                                wb_data = reg_file[rs2];
                                memory.at(load_addr_phy/8) = wb_data;
                            }
                            break;

                        case 0b00000 : //AMOADD.D
                            
                            if ((load_addr_phy%8)!=0){
                                cout << "AMO-SC.W : Mis-aligned memory access " << hex<< load_addr_phy%8<< endl;
                                mtval = load_addr;
                                STORE_ADDR_MISSALIG = true;
                            }
                            else {
                                ret_data = wb_data;
                                wb_data = (wb_data + reg_file[rs2]);
                                memory.at(load_addr_phy/8) = wb_data;
                            }
                            break;

                        case 0b00100 : //AMOXOR.D
                            
                            if ((load_addr_phy%8)!=0){
                                cout << "AMO-SC.W : Mis-aligned memory access " << hex<< load_addr_phy%8<< endl;
                                mtval = load_addr;
                                STORE_ADDR_MISSALIG = true;
                            }
                            else {
                                ret_data = wb_data;
                                wb_data = (wb_data ^ reg_file[rs2]);
                                memory.at(load_addr_phy/8) = wb_data;
                            }
                            break;

                        case 0b01100 : //AMOAND.D
                            
                            if ((load_addr_phy%8)!=0){
                                cout << "AMO-SC.W : Mis-aligned memory access " << hex<< load_addr_phy%8<< endl;
                                mtval = load_addr;
                                STORE_ADDR_MISSALIG = true;
                            }
                            else {
                                ret_data = wb_data;
                                wb_data = (wb_data & reg_file[rs2]);
                                memory.at(load_addr_phy/8) = wb_data;
                            }
                            break;

                        case 0b01000 : //AMOOR.D
                            
                            if ((load_addr_phy%8)!=0){
                                cout << "AMO-SC.W : Mis-aligned memory access " << hex<< load_addr_phy%8<< endl;
                                mtval = load_addr;
                                STORE_ADDR_MISSALIG = true;
                            }
                            else {
                                ret_data = wb_data;
                                wb_data = (wb_data | reg_file[rs2]);
                                memory.at(load_addr_phy/8) = wb_data;
                            }
                            break;

                        case 0b10000 : //AMOMIN.D
                            
                            if ((load_addr_phy%8)!=0){
                                cout << "AMO-SC.W : Mis-aligned memory access " << hex<< load_addr_phy%8<< endl;
                                mtval = load_addr;
                                STORE_ADDR_MISSALIG = true;
                            }
                            else {
                                ret_data = wb_data;
                                wb_data = min(signed_value(wb_data), signed_value(reg_file[rs2]));
                                memory.at(load_addr_phy/8) = wb_data;
                            }
                            break;

                        case 0b10100 : //AMOMAX.D
                            
                            if ((load_addr_phy%8)!=0){
                                cout << "AMO-SC.W : Mis-aligned memory access " << hex<< load_addr_phy%8<< endl;
                                mtval = load_addr;
                                STORE_ADDR_MISSALIG = true;
                            }
                            else {
                                ret_data = wb_data;
                                wb_data = max(signed_value(wb_data), signed_value(reg_file[rs2]));
                                memory.at(load_addr_phy/8) = wb_data;
                            }
                            break;

                        case 0b11000 : //AMOMINU.D
                            
                            if ((load_addr_phy%8)!=0){
                                cout << "AMO-SC.W : Mis-aligned memory access " << hex<< load_addr_phy%8<< endl;
                                mtval = load_addr;
                                STORE_ADDR_MISSALIG = true;
                            }
                            else {
                                ret_data = wb_data;
                                wb_data = min(wb_data, reg_file[rs2]);
                                memory.at(load_addr_phy/8) = wb_data;
                            }
                            break;

                        case 0b11100 : //AMOMAXU.D
                            
                            if ((load_addr_phy%8)!=0){
                                cout << "AMO-SC.W : Mis-aligned memory access " << hex<< load_addr_phy%8<< endl;
                                mtval = load_addr;
                                STORE_ADDR_MISSALIG = true;
                            }
                            else {
                                ret_data = wb_data;
                                wb_data = max(wb_data, reg_file[rs2]);
                                memory.at(load_addr_phy/8) = wb_data;
                            }
                            break;

                        default :  
                            printf("******INVALID INSTRUCTION******\nINS :%lu\nOPCODE :%lu\n",instruction,instruction & 0b1111111);
                            bitset<3> ins(func3);
                            cout<<  "func3 : "<<ins<<endl;
                            break;                        
                    }
                    reg_file[rd] = ret_data;
                }
                break;

            case fence :
                continue;

            case systm :
                if (imm11_0==CYCLE){
                    //cout << "culprit PC : "<<hex<<PC_phy<<endl;
                    //cout << func3 << endl;
                }
                switch(func3){
                    case 0b001 : // CSRRW
                        //csr_data = csr_file[imm11_0];
                        csr_data = csr_read(imm11_0);
                        if (csr_read_success){
                            store_data = reg_file[rs1];
                            //csr_file[imm11_0] = store_data;
                            csr_bool = csr_write(imm11_0,store_data);
                            if(!csr_bool){
                                mtval = instruction;
                                ILL_INS = true;
                            }
                            else if (rd!=0)
                                reg_file[rd] = csr_data;
                        } else{
                            mtval = instruction;
                            ILL_INS = true;
                        }
                        break;

                    case 0b010 : // CSRRS     rdtime, rdcycle, rdinsret should be handled here
                        //csr_data = csr_file[imm11_0];
                        csr_data = csr_read(imm11_0);
                        //cout << csr_read_success << endl;
                        if (csr_read_success) {
                            if ( (imm11_0==CYCLE) | (imm11_0==TIME) | (imm11_0==INSTRET) ){
                                // Acout << hex << imm11_0 << " read : "<<store_data<<endl;
                                reg_file[rd] = csr_data;
                            }
                            else {
                                store_data = reg_file[rs1];
                                store_data = (store_data | csr_data);
                                //csr_file[imm11_0] = store_data;
                                csr_bool = csr_write(imm11_0,store_data);
                                if(!csr_bool){
                                    mtval = instruction;
                                    ILL_INS = true;
                                } else 
                                    reg_file[rd] = csr_data;
                            }
                        } 
                        else{
                            mtval = instruction;
                            ILL_INS = true;
                        }
                        break;

                    case 0b011 : // CSRRC
                        //csr_data = csr_file[imm11_0];
                        csr_data = csr_read(imm11_0);
                        if (csr_read_success){
                            store_data = reg_file[rs1];
                            store_data = (csr_data & (MASK64 - store_data));
                            //csr_file[imm11_0] = store_data;
                            csr_bool = csr_write(imm11_0,store_data);
                            if(!csr_bool){
                                mtval = instruction;
                                ILL_INS = true;
                            } else 
                                reg_file[rd] = csr_data;
                        } else{
                            mtval = instruction;
                            ILL_INS = true;
                        }
                        break;

                    case 0b101 : // CSRRWI
                        //csr_data = csr_file[imm11_0];
                        csr_data = csr_read(imm11_0);
                        if (csr_read_success){
                            //csr_file[imm11_0] = rs1;
                            csr_bool = csr_write(imm11_0,rs1);
                            if(!csr_bool){
                                mtval = instruction;
                                ILL_INS = true;
                            }
                            else if (rd!=0)
                                reg_file[rd] = csr_data;
                        } else{
                            mtval = instruction;
                            ILL_INS = true;
                        }
                        break;

                    case 0b110 : // CSRRSI
                        //csr_data = csr_file[imm11_0];
                        csr_data = csr_read(imm11_0);
                        if (csr_read_success){
                            store_data = (rs1 | csr_data);
                            //csr_file[imm11_0] = store_data;
                            csr_bool = csr_write(imm11_0,store_data);
                            if(!csr_bool){
                                mtval = instruction;
                                ILL_INS = true;
                            } else 
                                reg_file[rd] = csr_data;
                        } else{
                            mtval = instruction;
                            ILL_INS = true;
                        }
                        break;

                    case 0b111 : // CSRRCI
                        //csr_data = csr_file[imm11_0];
                        csr_data = csr_read(imm11_0);
                        if (csr_read_success){
                            store_data = (csr_data & (MASK64 - rs1));
                            //csr_file[imm11_0] = store_data;
                            csr_bool = csr_write(imm11_0,store_data);
                            if(!csr_bool){
                                mtval = instruction;
                                ILL_INS = true;
                            } else 
                                reg_file[rd] = csr_data;
                        } else{
                            mtval = instruction;
                            ILL_INS = true;
                        }
                        break;

                    case 0b000 : 
                        switch(imm11_0){
                            case 0 : //ecall
                        
                                PC = excep_function(PC,CAUSE_MACHINE_ECALL,CAUSE_SUPERVISOR_ECALL,CAUSE_USER_ECALL,cp);
                                break;

                            case 1 : //ebreak
                                //PC = excep_function(PC,CAUSE_BREAKPOINT,CAUSE_BREAKPOINT,CAUSE_BREAKPOINT,cp);
                                EBREAK = true;
                                break;

                            case 770 : //mret
                                PC = mepc;
                                cp = (plevel_t)mstatus.mpp;

                                mstatus.mie = 1;//mstatus.mpie;
                                mstatus.mpp = 0b00; //setting to umode
                                mstatus.mpie = 0;
                                break;

                            case 258 : //sret
                                PC = sepc;
                                cp = (plevel_t)mstatus.spp;
                                mstatus.mpp = 0b00; //setting to umode
                                mstatus.spp = 0b0;
                                sstatus.sie = sstatus.spie;
                                sstatus.spie = 1;
                                break;

                            case 2 : //uret
                                PC = uepc;
                                cp = (plevel_t)UMODE;
                                mstatus.mpp = 0b00; //setting to umode
                                mstatus.spp = 0b0;
                                ustatus.uie = ustatus.upie;
                                ustatus.upie = 1;
                                break;

                            default :
                                // cout << "Invalid EXCEP"<<endl;
                                break;
                        }
                        break;

                    default :
                        cout << "Invalid system instruction : "<< func3 << endl;
                        break;
                }
                break;

            case fd1 :
                cout << "FD Instructions"<<endl;
                mtval = instruction;
                ILL_INS = true;
                break;

            case fd2 :
                cout << "FD Instructions"<<endl;
                mtval = instruction;
                ILL_INS = true;
                break;

            case fd3 :
                cout << "FD Instructions"<<endl;
                mtval = instruction;
                ILL_INS = true;
                break;

            case fd4 :
                cout << "FD Instructions"<<endl;
                mtval = instruction;
                ILL_INS = true;
                break;

            case fd5 :
                cout << "FD Instructions"<<endl;
                mtval = instruction;
                ILL_INS = true;
                break;

            case fd6 :
                cout << "FD Instructions"<<endl;
                mtval = instruction;
                ILL_INS = true;
                break;

            case fd7 :
                cout << "FD Instructions"<<endl;
                mtval = instruction;
                ILL_INS = true; 
                break;

            default :
                printf("default\n");
                bitset<32> ins1(instruction);
                cout << "Instruction : "<<ins1 << endl;
                cout << "PC : " << PC-4 <<endl;
                break;
        }
        

        if (mstatus.fs==3){
            mstatus.sd = 1;
        }
        else {
            mstatus.sd = 0;
        }

        //if (PC >= ((1llu)<<MEM_SIZE)){ //instruction access exception
        //    mtval = PC;
        //    PC = excep_function(PC,CAUSE_FETCH_ACCESS,CAUSE_FETCH_ACCESS,CAUSE_FETCH_ACCESS,cp);   
        //}

        if (lPC==PC){
            //infinite loop
            cout << "Infinite loop!"<<(int)mip.STIP<<endl;
            break;
        }
        //cout << "mtime    : "<<mtime<<endl;
        //cout << "mtimecmp : "<<mtimecmp<<endl;
        //cout << "mstatus.mie : "<< (uint_t)mstatus.mie <<endl;
         //timer interrupt
            // mip.MTIP = (mtime >= mtimecmp );

            //cout << "cp : "<< (uint_t)cp <<endl;
            //cout << "sie : "<<(uint_t)mstatus.sie<<endl;

        //exception/interupt finding combo

        //external interupts >> software interupts >> timer interupts >> synchornous traps
      
        if(LD_ACC_FAULT) {
            cout << "This should not occur"<<endl;
            LD_ACC_FAULT = false;
            PC = excep_function(PC,CAUSE_LOAD_ACCESS,CAUSE_LOAD_ACCESS,CAUSE_LOAD_ACCESS,cp);
            write_tval = false;
        }
       
     
      
        else if( mie.MEIE & mip.MEIP) {

            PC = interrupt_function(PC, CAUSE_MACHINE_EXT_INT, cp);
        }
        else if( mie.MTIE & mip.MTIP) {
            //  if(mstatus.mie) {
            //     cout<<"timer interrupt"<<hex<<(int)mip.STIP<<endl;
            // }
            // //cout << "because of this"<<hex<<cp<<endl;
            //cout << "mstatus.mie : "<<(uint_t)mstatus.mie<<endl; 
            //exit(0);
            PC = interrupt_function(PC, CAUSE_MACHINE_TIMER_INT, cp);
        }
        else if( mie.MSIE & mip.MSIP) {
            PC = interrupt_function(PC, CAUSE_MACHINE_SOFT_INT, cp);
        }
        else if( mie.SEIE & mip.SEIP) {
            PC = interrupt_function(PC, CAUSE_SUPERVISOR_EXT_INT, cp);  
        }
        // else if(ECALL) {
        //     ECALL = false;
        //     PC = excep_function(PC,CAUSE_MACHINE_ECALL,CAUSE_SUPERVISOR_ECALL,CAUSE_USER_ECALL,cp);
        //     write_tval = false;
        // }
        else if( mie.STIE & mip.STIP) {
            PC = interrupt_function(PC, CAUSE_SUPERVISOR_TIMER_INT, cp);
        }
        else if( mie.SSIE & mip.SSIP) {
             PC = interrupt_function(PC, CAUSE_SUPERVISOR_SOFT_INT, cp);
        }
        else if( mie.UEIE & mip.UEIP) {
                PC = interrupt_function(PC, CAUSE_USER_EXT_INT, cp);
        }
        else if( mie.UTIE & mip.UTIP) {
             PC = interrupt_function(PC, CAUSE_USER_TIMER_INT, cp);
        }
        else if( mie.USIE & mip.USIP) {
            PC = interrupt_function(PC, CAUSE_USER_SOFT_INT, cp);
        }
        
    
        else if(INS_ACC_FAULT) {
            INS_ACC_FAULT = false;
            PC = excep_function(PC,CAUSE_FETCH_ACCESS,CAUSE_FETCH_ACCESS,CAUSE_FETCH_ACCESS,cp);
            write_tval = false;
        }
        
        else if (ILL_INS) {
            ILL_INS = false;
            PC = excep_function(PC,CAUSE_ILLEGAL_INSTRUCTION,CAUSE_ILLEGAL_INSTRUCTION,CAUSE_ILLEGAL_INSTRUCTION,cp);
            write_tval = true;
        }
        else if(EBREAK) {
            EBREAK = false;
            PC = excep_function(PC,CAUSE_BREAKPOINT,CAUSE_BREAKPOINT,CAUSE_BREAKPOINT,cp);
            write_tval = false;
        }
        else if(INS_ADDR_MISSALIG) {
            INS_ADDR_MISSALIG = false;
            PC = excep_function(PC,CAUSE_MISALIGNED_FETCH,CAUSE_MISALIGNED_FETCH,CAUSE_MISALIGNED_FETCH,cp);
            write_tval = true;
        }
        else if (LD_ADDR_MISSALIG) {
            LD_ADDR_MISSALIG = false;
            PC = excep_function(PC,CAUSE_MISALIGNED_LOAD,CAUSE_MISALIGNED_LOAD,CAUSE_MISALIGNED_LOAD,cp);
            write_tval = true;
        }
        else if(STORE_ADDR_MISSALIG) {
            STORE_ADDR_MISSALIG = false;
            PC = excep_function(PC,CAUSE_MISALIGNED_STORE,CAUSE_MISALIGNED_STORE,CAUSE_MISALIGNED_STORE,cp);
            write_tval = true;
        }

        if(write_tval){
            write_tval = false;
            switch(cp){
                case MMODE : 
                    mtval = mtval;
                    break;
                case SMODE :
                    stval = mtval;
                    mtval = 0;
                    break;
                case UMODE :
                    utval = mtval;
                    mtval = 0;
                    break;
            }
        }

/*
        if (mip.MTIP == 0b1){
            switch(cp) {
                case MMODE : 
                    if ((mie.MTIE == 0b1) & (mstatus.mie==0b1)){
                        //cout << "\nM : Timer interrupt fired" <<endl;
                        //print_reg_file(reg_file);
                        PC = interrupt_function(PC, 7, 5, 4, cp);
                        //mstatus.mie = 0b0;
                    }
                    break;
                case SMODE : 
                    if ((mie.STIE == 0b1) & (mstatus.sie==0b1)){
                        //cout << "\nS : Timer interrupt fired" <<endl;
                        PC = interrupt_function(PC, 7, 5, 4, cp);
                        //mstatus.sie = 0b0;
                    }
                    //cout << "S : not set" <<endl;
                    break;
                case UMODE : 
                    if ((mie.UTIE == 0b1) & (mstatus.uie==0b1)){
                        //cout << "\nU : Timer interrupt fired" <<endl;
                        PC = interrupt_function(PC, 7, 5, 4, cp);
                        //mstatus.uie = 0b0;
                    }
                    break;
                default : 
                    cout << "illegel mode for timer intterupt"<<endl;
                    break;
            }
        }*/

    }

    return 0;
}

//mcycle, ins

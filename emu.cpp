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


#include "csr_file.h"
#include "emu.h"

using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds
using namespace std;


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


int main(){

    vector<uint_t> memory(1<<MEM_SIZE); // main memory

    vector<uint_t> reg_file(32);       // register file

    ifstream infile("data_hex.txt");
    string line;

    uint_t i = 0;

    uint_t PC = 0;
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
    plevel_t cp     = (plevel_t)MMODE;
    plevel_t LR_cp;
    uint_t LR_count = 0;

    plevel_t LR_cp64;
    uint_t LR_count64 = 0;

    uint_t amo_op   = 0;
    bool amo_reserve_valid = false;
    bool amo_reserve_valid64 = false;
    uint_t amo_reserve_addr = 0;
    uint_t amo_reserve_addr64 = 0;

    //initializing reg file
    reg_file[2]  = 0x00400000 ; //SP
    reg_file[11] = 0x00010000 ;

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

    while (PC < ((1llu)<<MEM_SIZE)){

        #ifdef DEBUG
            //sleep_for(milliseconds(500));
            cout << "PC : "<< hex << PC << endl;
        #endif
        //sleep_for(milliseconds(10));

        PC_phy = translate(PC, INST, cp);
        if (PC_phy==-1){
            PC = excep_function(PC,CAUSE_FETCH_PAGE_FAULT,CAUSE_FETCH_PAGE_FAULT,CAUSE_FETCH_PAGE_FAULT,cp);
            continue;
        }

        instruction = getINST(PC_phy/4,&memory);


        reg_file[0] = 0;

        #ifdef DEBUG
            bitset<32> ins(instruction);
            cout << "Instruction : "<<ins << endl;
        #endif

        opcode = static_cast<opcode_t>((instruction) & 0b1111111);

        wb_data = 0;

        rd      = ((instruction) >> 7 ) & 0b11111   ;
        func3   = ((instruction) >> 12) & 0b111     ;
        rs1     = ((instruction) >> 15) & 0b11111   ;
        rs2     = ((instruction) >> 20) & 0b11111   ;
        func7   = ((instruction) >> 25) & 0b1111111 ;

        imm11_0  = ((instruction) >> 20) & 0b111111111111 ;
        imm31_12 = ((instruction) >> 12) & 1048575 ;       // extract 20 bits

        imm_j    = ((((instruction)>>31) & 0b1)<<20) + ((instruction) & (0b11111111<<12)) + ((((instruction)>>20) & 0b1)<<11) + ((((instruction)>>21) & 0b1111111111)<<1); //((instruction>>31) & 0b1)<<20 + (instruction & (0b11111111<<12)) + ((instruction>>20) & 0b1)<<11 +
        imm_b    = ((((instruction)>>31) & 0b1)<<12) + ((((instruction)>>7) & 0b1)<<11) + ((((instruction)>>25) & 0b111111)<<5) + (((instruction)>>7) & 0b11110) ;
        imm_s    = ((((instruction)>>25) & 0b1111111)<<5) + (((instruction)>>7) & 0b11111) ;

        amo_op   = ((instruction) >> 27) & 0b11111 ;


        if (amo_reserve_valid64){
            LR_count64 +=1;
            if ( (opcode==jump) | (opcode==jumpr) | (opcode==cjump) | (opcode==load) | (opcode==store) | (opcode==fence) | (opcode==systm) ){
                cout << "Illegal instruction in between LR/SC 64 : " << (uint_t)opcode << endl;
                amo_reserve_valid64 = false;
                amo_reserve_addr64 = 0;
                LR_count64 = 0;
            }
            else if (LR_cp64 != cp){
                cout << "Privilege changed in between LR/SC 64 : " << endl;
                amo_reserve_valid64 = false;
                amo_reserve_addr64 = 0;
                LR_count64 = 0;
            }
        }

        if (amo_reserve_valid){
            LR_count +=1;
            if ( (opcode==jump) | (opcode==jumpr) | (opcode==cjump) | (opcode==load) | (opcode==store) | (opcode==fence) | (opcode==systm) ){
                cout << "Illegal instruction in between LR/SC : " << (uint_t)opcode << endl;
                amo_reserve_valid = false;
                amo_reserve_addr = 0;
                LR_count = 0;
            }
            else if (LR_cp != cp){
                cout << "Privilege changed in between LR/SC : " << endl;
                amo_reserve_valid = false;
                amo_reserve_addr = 0;
                LR_count = 0;
            }
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
                    case 0b000 : branch = (reg_file[rs1] == reg_file[rs2]); break; //BEQ

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
                    if (load_addr >= ((1llu)<<MEM_SIZE)){ //memory access exception
                        mtval = load_addr;
                        PC = excep_function(PC,CAUSE_LOAD_ACCESS,CAUSE_LOAD_ACCESS,CAUSE_LOAD_ACCESS,cp);   
                    }
                    else{
                        load_addr_phy = translate(load_addr, LOAD, cp);
                        if (load_addr_phy==-1){
                            PC = excep_function(PC,CAUSE_LOAD_PAGE_FAULT,CAUSE_LOAD_PAGE_FAULT,CAUSE_LOAD_PAGE_FAULT,cp);
                            continue;
                        }
                        load_data = memory.at(load_addr_phy/8);
                        switch(func3){
                            case 0b000 : 
                                if (!load_byte(load_addr_phy,load_data, wb_data)){
                                    PC = excep_function(PC,CAUSE_MISALIGNED_LOAD,CAUSE_MISALIGNED_LOAD,CAUSE_MISALIGNED_LOAD,cp);
                                    mtval = load_addr;                                  
                                } else {
                                    wb_data = sign_extend<uint_t>(wb_data & (0xFF)      , 8); 
                                    reg_file[rd] = wb_data;
                                }
                                break; //LB sign extend  8 bit value

                            case 0b001 : 
                                if (!load_halfw(load_addr_phy,load_data, wb_data)){
                                    PC = excep_function(PC,CAUSE_MISALIGNED_LOAD,CAUSE_MISALIGNED_LOAD,CAUSE_MISALIGNED_LOAD,cp);
                                    mtval = load_addr;
                                } else {
                                    wb_data = sign_extend<uint_t>(wb_data & (0xFFFF)    ,16); 
                                    reg_file[rd] = wb_data;
                                }
                                break; //LH sign extend 16 bit value

                            case 0b010 : 
                                if (!load_word(load_addr_phy,load_data, wb_data)){
                                    PC = excep_function(PC,CAUSE_MISALIGNED_LOAD,CAUSE_MISALIGNED_LOAD,CAUSE_MISALIGNED_LOAD,cp);
                                    mtval = load_addr;
                                    //cout << "LW"<<endl;
                                } else {
                                    wb_data = sign_extend<uint_t>(wb_data & (0xFFFFFFFF),32); 
                                    reg_file[rd] = wb_data;
                                }
                                break; //LW sign extend 32 bit value

                            case 0b100 : 
                                if (!load_byte(load_addr_phy,load_data, wb_data)){
                                    PC = excep_function(PC,CAUSE_MISALIGNED_LOAD,CAUSE_MISALIGNED_LOAD,CAUSE_MISALIGNED_LOAD,cp);
                                    mtval = load_addr;
                                    //cout << "LBU" <<hex<<load_addr_phy<<endl;
                                } else {
                                    wb_data = wb_data & 0xFF      ; 
                                    reg_file[rd] = wb_data;
                                }
                                break; //LBU zero extend  8 bit value

                            case 0b101 : 
                                if (!load_halfw(load_addr_phy,load_data, wb_data)){
                                    PC = excep_function(PC,CAUSE_MISALIGNED_LOAD,CAUSE_MISALIGNED_LOAD,CAUSE_MISALIGNED_LOAD,cp);
                                    mtval = load_addr;
                                    //cout << "LHU"<<endl;
                                } else {
                                    wb_data = wb_data & 0xFFFF    ; 
                                    reg_file[rd] = wb_data;
                                }
                                break; //LHU zero extend 16 bit value

                            case 0b110 : 
                                if (!load_word(load_addr_phy,load_data, wb_data)){
                                    PC = excep_function(PC,CAUSE_MISALIGNED_LOAD,CAUSE_MISALIGNED_LOAD,CAUSE_MISALIGNED_LOAD,cp);
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
                                    PC = excep_function(PC,CAUSE_MISALIGNED_LOAD,CAUSE_MISALIGNED_LOAD,CAUSE_MISALIGNED_LOAD,cp);
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
                } else if (((reg_file[rs1] + sign_extend<uint_t>(imm11_0,12))) == FIFO_ADDR_RX) {
                    wb_data = 0 ;
                    reg_file[rd] = wb_data;
                }
                else if (((reg_file[rs1] + sign_extend<uint_t>(imm11_0,12))) == FIFO_ADDR_TX){
                    wb_data = (uint_t)getchar() ;
                    reg_file[rd] = wb_data;
                }
                break;

            case store :
                #ifdef DEBUG
                    printf("STORE\n");
                #endif
                store_addr = reg_file[rs1] + sign_extend<uint_t>(imm_s,12);
                if (store_addr != FIFO_ADDR_TX){
                    if (store_addr >= ((1llu)<<MEM_SIZE)){ //memory access exception
                        cout << "Mem access exception"<<endl;
                        mtval = store_addr;
                        PC = excep_function(PC,CAUSE_STORE_ACCESS,CAUSE_STORE_ACCESS,CAUSE_STORE_ACCESS,cp);   
                    }
                    else {
                        store_addr_phy = translate(store_addr, STOR, cp);
                        if (store_addr_phy==-1){
                            cout << "Page fault exception"<<endl;
                            PC = excep_function(PC,CAUSE_STORE_PAGE_FAULT,CAUSE_STORE_PAGE_FAULT,CAUSE_STORE_PAGE_FAULT,cp);
                            continue;
                        }
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
                            PC = excep_function(PC,CAUSE_MISALIGNED_STORE,CAUSE_MISALIGNED_STORE,CAUSE_MISALIGNED_STORE,cp);
                            mtval = store_addr;
                        }else {
                            memory.at(store_addr_phy/8) = wb_data;
                        }
                    }

                } else {
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
                    switch (amo_op){
                        case 0b00010 : //LR.W
                            load_addr = reg_file[rs1];
                            load_data = memory.at(load_addr/8);
                            ls_success = load_word(load_addr,load_data, wb_data);
                            if (!ls_success){
                                cout << "AMO-LR.W : Mis-aligned memory access" << endl;
                                PC = excep_function(PC,CAUSE_MISALIGNED_STORE,CAUSE_MISALIGNED_STORE,CAUSE_MISALIGNED_STORE,cp);
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
                                store_addr = reg_file[rs1];
                                load_data  = memory.at(store_addr/8);

                                ls_success = store_word(store_addr, load_data, store_data, wb_data);
                                if (!ls_success){
                                    cout << "AMO-SC.W : Mis-aligned memory access" << endl;
                                    PC = excep_function(PC,CAUSE_MISALIGNED_STORE,CAUSE_MISALIGNED_STORE,CAUSE_MISALIGNED_STORE,cp);
                                    ret_data = 1;
                                }
                                else {
                                    memory.at(store_addr/8) = wb_data;
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
                            load_addr = reg_file[rs1];
                            load_data = memory.at(load_addr/8);
                            ls_success = load_word(load_addr,load_data, wb_data);
                            if (!ls_success){
                                cout << "AMO-SC.W : Mis-aligned memory access" << endl;
                                PC = excep_function(PC,CAUSE_MISALIGNED_STORE,CAUSE_MISALIGNED_STORE,CAUSE_MISALIGNED_STORE,cp);
                            }
                            else {
                                ret_data = sign_extend<uint_t>((wb_data & MASK32),32);
                                wb_data = reg_file[rs2] & MASK32;
                                ls_success = store_word(load_addr,load_data,wb_data, store_data);
                                memory.at(load_addr/8) = store_data;
                            }
                            break;

                        case 0b00000 : //AMOADD.W
                            load_addr = reg_file[rs1];
                            load_data = memory.at(load_addr/8);
                            ls_success = load_word(load_addr,load_data, wb_data);
                            if (!ls_success){
                                cout << "AMO-SC.W : Mis-aligned memory access" << endl;
                                PC = excep_function(PC,CAUSE_MISALIGNED_STORE,CAUSE_MISALIGNED_STORE,CAUSE_MISALIGNED_STORE,cp);
                            }
                            else {
                                ret_data = sign_extend<uint_t>((wb_data & MASK32),32);
                                wb_data = ((wb_data & MASK32) + (reg_file[rs2] & MASK32)) & MASK32;
                                ls_success = store_word(load_addr,load_data,wb_data, store_data);
                                memory.at(load_addr/8) = store_data;
                            }
                            break;

                        case 0b00100 : //AMOXOR.W
                            load_addr = reg_file[rs1];
                            load_data = memory.at(load_addr/8);
                            ls_success = load_word(load_addr,load_data, wb_data);
                            if (!ls_success){
                                cout << "AMO-SC.W : Mis-aligned memory access" << endl;
                                PC = excep_function(PC,CAUSE_MISALIGNED_STORE,CAUSE_MISALIGNED_STORE,CAUSE_MISALIGNED_STORE,cp);
                            }
                            else {
                                ret_data = sign_extend<uint_t>((wb_data & MASK32),32);
                                wb_data = ((wb_data & MASK32) ^ (reg_file[rs2] & MASK32)) & MASK32;
                                ls_success = store_word(load_addr,load_data,wb_data, store_data);
                                memory.at(load_addr/8) = store_data;
                            }
                            break;

                        case 0b01100 : //AMOAND.W
                            load_addr = reg_file[rs1];
                            load_data = memory.at(load_addr/8);
                            ls_success = load_word(load_addr,load_data, wb_data);
                            if (!ls_success){
                                cout << "AMO-SC.W : Mis-aligned memory access" << endl;
                                PC = excep_function(PC,CAUSE_MISALIGNED_STORE,CAUSE_MISALIGNED_STORE,CAUSE_MISALIGNED_STORE,cp);
                            }
                            else {
                                ret_data = sign_extend<uint_t>((wb_data & MASK32),32);
                                wb_data = ((wb_data & MASK32) & (reg_file[rs2] & MASK32)) & MASK32;
                                ls_success = store_word(load_addr,load_data,wb_data, store_data);
                                memory.at(load_addr/8) = store_data;
                            }
                            break;

                        case 0b01000 : //AMOOR.W
                            load_addr = reg_file[rs1];
                            load_data = memory.at(load_addr/8);
                            ls_success = load_word(load_addr,load_data, wb_data);
                            if (!ls_success){
                                cout << "AMO-SC.W : Mis-aligned memory access" << endl;
                                PC = excep_function(PC,CAUSE_MISALIGNED_STORE,CAUSE_MISALIGNED_STORE,CAUSE_MISALIGNED_STORE,cp);
                            }
                            else {
                                ret_data = sign_extend<uint_t>((wb_data & MASK32),32);
                                wb_data = ((wb_data & MASK32) | (reg_file[rs2] & MASK32)) & MASK32;
                                ls_success = store_word(load_addr,load_data,wb_data, store_data);
                                memory.at(load_addr/8) = store_data;
                            }
                            break;

                        case 0b10000 : //AMOMIN.W
                            load_addr = reg_file[rs1];
                            load_data = memory.at(load_addr/8);
                            ls_success = load_word(load_addr,load_data, wb_data);
                            if (!ls_success){
                                cout << "AMO-SC.W : Mis-aligned memory access" << endl;
                                PC = excep_function(PC,CAUSE_MISALIGNED_STORE,CAUSE_MISALIGNED_STORE,CAUSE_MISALIGNED_STORE,cp);
                            }
                            else {
                                ret_data = sign_extend<uint_t>((wb_data & MASK32),32);
                                wb_data = min(signed_value32(wb_data & MASK32), signed_value32(reg_file[rs2] & MASK32)) & MASK32;
                                ls_success = store_word(load_addr,load_data,wb_data, store_data);
                                memory.at(load_addr/8) = store_data;
                            }
                            break;

                        case 0b10100 : //AMOMAX.W
                            load_addr = reg_file[rs1];
                            load_data = memory.at(load_addr/8);
                            ls_success = load_word(load_addr,load_data, wb_data);
                            if (!ls_success){
                                cout << "AMO-SC.W : Mis-aligned memory access" << endl;
                                PC = excep_function(PC,CAUSE_MISALIGNED_STORE,CAUSE_MISALIGNED_STORE,CAUSE_MISALIGNED_STORE,cp);
                            }
                            else {
                                ret_data = sign_extend<uint_t>((wb_data & MASK32),32);
                                wb_data = max(signed_value32(wb_data & MASK32), signed_value32(reg_file[rs2] & MASK32)) & MASK32;
                                ls_success = store_word(load_addr,load_data,wb_data, store_data);
                                memory.at(load_addr/8) = store_data;
                            }
                            break;

                        case 0b11000 : //AMOMINU.W
                            load_addr = reg_file[rs1];
                            load_data = memory.at(load_addr/8);
                            ls_success = load_word(load_addr,load_data, wb_data);
                            if (!ls_success){
                                cout << "AMO-SC.W : Mis-aligned memory access" << endl;
                                PC = excep_function(PC,CAUSE_MISALIGNED_STORE,CAUSE_MISALIGNED_STORE,CAUSE_MISALIGNED_STORE,cp);
                            }
                            else {
                                ret_data = sign_extend<uint_t>((wb_data & MASK32),32);
                                wb_data = min((wb_data & MASK32), (reg_file[rs2] & MASK32)) & MASK32;
                                ls_success = store_word(load_addr,load_data,wb_data, store_data);
                                memory.at(load_addr/8) = store_data;
                            }
                            break;

                        case 0b11100 : //AMOMAXU.W
                            load_addr = reg_file[rs1];
                            load_data = memory.at(load_addr/8);
                            ls_success = load_word(load_addr,load_data, wb_data);
                            if (!ls_success){
                                cout << "AMO-SC.W : Mis-aligned memory access" << endl;
                                PC = excep_function(PC,CAUSE_MISALIGNED_STORE,CAUSE_MISALIGNED_STORE,CAUSE_MISALIGNED_STORE,cp);
                            }
                            else {
                                ret_data = sign_extend<uint_t>((wb_data & MASK32),32);
                                wb_data = max((wb_data & MASK32), (reg_file[rs2] & MASK32)) & MASK32;
                                ls_success = store_word(load_addr,load_data,wb_data, store_data);
                                memory.at(load_addr/8) = store_data;
                            }
                            break;

                        default :
                            printf("******INVALID INSTRUCTION******\nINS :%lu\nOPCODE :%lu\n",instruction,(uint_t)opcode);
                            bitset<5> ins(amo_op);
                            cout<<  "amo op : "<<ins<<endl;
                            PC = excep_function(PC,CAUSE_ILLEGAL_INSTRUCTION,CAUSE_ILLEGAL_INSTRUCTION,CAUSE_ILLEGAL_INSTRUCTION,cp);
                            break;   
                    }
                    reg_file[rd] = ret_data;
                }
                else if (func3 == 0b011){
                    #ifdef DEBUG
                        printf("AMO 64\n");
                    #endif
                    switch (amo_op){ 
                        case 0b00010 : //LR.D
                            load_addr = reg_file[rs1];
                            wb_data = memory.at(load_addr/8);
                            if ((load_addr%8)!=0){
                                cout << "AMO-LR.D : Mis-aligned memory access" << endl;
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
                                store_addr = reg_file[rs1];
                                if ((store_addr%8)!=0){
                                    cout << "AMO-SC.D : Mis-aligned memory access" << endl;
                                }
                                else {
                                    memory.at(store_addr/8) = store_data;
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
                            load_addr = reg_file[rs1];
                            wb_data = memory.at(load_addr/8);
                            if ((store_addr%8)!=0){
                                cout << "AMO-SC.W : Mis-aligned memory access" << endl;
                            }
                            else {
                                ret_data = wb_data;
                                wb_data = reg_file[rs2];
                                memory.at(load_addr/8) = wb_data;
                            }
                            break;

                        case 0b00000 : //AMOADD.D
                            load_addr = reg_file[rs1];
                            wb_data = memory.at(load_addr/8);
                            if ((store_addr%8)!=0){
                                cout << "AMO-SC.W : Mis-aligned memory access" << endl;
                            }
                            else {
                                ret_data = wb_data;
                                wb_data = (wb_data + reg_file[rs2]);
                                memory.at(load_addr/8) = wb_data;
                            }
                            break;

                        case 0b00100 : //AMOXOR.D
                            load_addr = reg_file[rs1];
                            wb_data = memory.at(load_addr/8);
                            if ((store_addr%8)!=0){
                                cout << "AMO-SC.W : Mis-aligned memory access" << endl;
                            }
                            else {
                                ret_data = wb_data;
                                wb_data = (wb_data ^ reg_file[rs2]);
                                memory.at(load_addr/8) = wb_data;
                            }
                            break;

                        case 0b01100 : //AMOAND.D
                            load_addr = reg_file[rs1];
                            wb_data = memory.at(load_addr/8);
                            if ((store_addr%8)!=0){
                                cout << "AMO-SC.W : Mis-aligned memory access" << endl;
                            }
                            else {
                                ret_data = wb_data;
                                wb_data = (wb_data & reg_file[rs2]);
                                memory.at(load_addr/8) = wb_data;
                            }
                            break;

                        case 0b01000 : //AMOOR.D
                            load_addr = reg_file[rs1];
                            wb_data = memory.at(load_addr/8);
                            if ((store_addr%8)!=0){
                                cout << "AMO-SC.W : Mis-aligned memory access" << endl;
                            }
                            else {
                                ret_data = wb_data;
                                wb_data = (wb_data | reg_file[rs2]);
                                memory.at(load_addr/8) = wb_data;
                            }
                            break;

                        case 0b10000 : //AMOMIN.D
                            load_addr = reg_file[rs1];
                            wb_data = memory.at(load_addr/8);
                            if ((store_addr%8)!=0){
                                cout << "AMO-SC.W : Mis-aligned memory access" << endl;
                            }
                            else {
                                ret_data = wb_data;
                                wb_data = min(signed_value(wb_data), signed_value(reg_file[rs2]));
                                memory.at(load_addr/8) = wb_data;
                            }
                            break;

                        case 0b10100 : //AMOMAX.D
                            load_addr = reg_file[rs1];
                            wb_data = memory.at(load_addr/8);
                            if ((store_addr%8)!=0){
                                cout << "AMO-SC.W : Mis-aligned memory access" << endl;
                            }
                            else {
                                ret_data = wb_data;
                                wb_data = max(signed_value(wb_data), signed_value(reg_file[rs2]));
                                memory.at(load_addr/8) = wb_data;
                            }
                            break;

                        case 0b11000 : //AMOMINU.D
                            load_addr = reg_file[rs1];
                            wb_data = memory.at(load_addr/8);
                            if ((store_addr%8)!=0){
                                cout << "AMO-SC.W : Mis-aligned memory access" << endl;
                            }
                            else {
                                ret_data = wb_data;
                                wb_data = min(wb_data, reg_file[rs2]);
                                memory.at(load_addr/8) = wb_data;
                            }
                            break;

                        case 0b11100 : //AMOMAXU.D
                            load_addr = reg_file[rs1];
                            wb_data = memory.at(load_addr/8);
                            if ((store_addr%8)!=0){
                                cout << "AMO-SC.W : Mis-aligned memory access" << endl;
                            }
                            else {
                                ret_data = wb_data;
                                wb_data = max(wb_data, reg_file[rs2]);
                                memory.at(load_addr/8) = wb_data;
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
                switch(func3){
                    case 0b001 : // CSRRW
                        //csr_data = csr_file[imm11_0];
                        csr_data = csr_read(imm11_0);
                        store_data = reg_file[rs1];
                        //csr_file[imm11_0] = store_data;
                        csr_write(imm11_0,store_data);
                        if (rd!=0)
                            reg_file[rd] = csr_data;
                        break;

                    case 0b010 : // CSRRS
                        //csr_data = csr_file[imm11_0];
                        csr_data = csr_read(imm11_0);
                        store_data = reg_file[rs1];
                        store_data = (store_data | csr_data);
                        //csr_file[imm11_0] = store_data;
                        csr_write(imm11_0,store_data);
                        reg_file[rd] = csr_data;
                        break;

                    case 0b011 : // CSRRC
                        //csr_data = csr_file[imm11_0];
                        csr_data = csr_read(imm11_0);
                        store_data = reg_file[rs1];
                        store_data = (csr_data & (MASK64 - store_data));
                        //csr_file[imm11_0] = store_data;
                        csr_write(imm11_0,store_data);
                        reg_file[rd] = csr_data;
                        break;

                    case 0b101 : // CSRRWI
                        //csr_data = csr_file[imm11_0];
                        csr_data = csr_read(imm11_0);
                        //csr_file[imm11_0] = rs1;
                        csr_write(imm11_0,rs1);
                        if (rd!=0)
                            reg_file[rd] = csr_data;
                        break;

                    case 0b110 : // CSRRSI
                        //csr_data = csr_file[imm11_0];
                        csr_data = csr_read(imm11_0);
                        store_data = (rs1 | csr_data);
                        //csr_file[imm11_0] = store_data;
                        csr_write(imm11_0,store_data);
                        reg_file[rd] = csr_data;
                        break;

                    case 0b111 : // CSRRCI
                        //csr_data = csr_file[imm11_0];
                        csr_data = csr_read(imm11_0);
                        store_data = (csr_data & (MASK64 - rs1));
                        //csr_file[imm11_0] = store_data;
                        csr_write(imm11_0,store_data);
                        reg_file[rd] = csr_data;
                        break;

                    case 0b000 : 
                        switch(imm11_0){
                            case 0 : //ecall
                                PC = excep_function(PC,CAUSE_MACHINE_ECALL,CAUSE_SUPERVISOR_ECALL,CAUSE_USER_ECALL,cp);
                                break;

                            case 1 : //ebreak
                                PC = excep_function(PC,CAUSE_BREAKPOINT,CAUSE_BREAKPOINT,CAUSE_BREAKPOINT,cp);
                                break;

                            case 770 : //mret
                                PC = mepc;
                                cp = (plevel_t)MMODE;
                                mstatus.mpp = 0b00; //setting to umode
                                mstatus.mie = mstatus.mpie;
                                mstatus.mpie = 1;
                                break;

                            case 258 : //sret
                                PC = sepc;
                                cp = (plevel_t)SMODE;
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
                                cout << "Invalid EXCEP"<<endl;
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
                PC = excep_function(PC,CAUSE_ILLEGAL_INSTRUCTION,CAUSE_ILLEGAL_INSTRUCTION,CAUSE_ILLEGAL_INSTRUCTION,cp);
                break;

            case fd2 :
                cout << "FD Instructions"<<endl;
                mtval = instruction;
                PC = excep_function(PC,CAUSE_ILLEGAL_INSTRUCTION,CAUSE_ILLEGAL_INSTRUCTION,CAUSE_ILLEGAL_INSTRUCTION,cp);
                break;

            case fd3 :
                cout << "FD Instructions"<<endl;
                mtval = instruction;
                PC = excep_function(PC,CAUSE_ILLEGAL_INSTRUCTION,CAUSE_ILLEGAL_INSTRUCTION,CAUSE_ILLEGAL_INSTRUCTION,cp);
                break;

            case fd4 :
                cout << "FD Instructions"<<endl;
                mtval = instruction;
                PC = excep_function(PC,CAUSE_ILLEGAL_INSTRUCTION,CAUSE_ILLEGAL_INSTRUCTION,CAUSE_ILLEGAL_INSTRUCTION,cp);
                break;

            case fd5 :
                cout << "FD Instructions"<<endl;
                mtval = instruction;
                PC = excep_function(PC,CAUSE_ILLEGAL_INSTRUCTION,CAUSE_ILLEGAL_INSTRUCTION,CAUSE_ILLEGAL_INSTRUCTION,cp);
                break;

            case fd6 :
                cout << "FD Instructions"<<endl;
                mtval = instruction;
                PC = excep_function(PC,CAUSE_ILLEGAL_INSTRUCTION,CAUSE_ILLEGAL_INSTRUCTION,CAUSE_ILLEGAL_INSTRUCTION,cp);
                break;

            case fd7 :
                cout << "FD Instructions"<<endl;
                mtval = instruction;
                PC = excep_function(PC,CAUSE_ILLEGAL_INSTRUCTION,CAUSE_ILLEGAL_INSTRUCTION,CAUSE_ILLEGAL_INSTRUCTION,cp); 
                break;

            default :
                printf("default\n");
                bitset<32> ins1(instruction);
                cout << "Instruction : "<<ins1 << endl;
                cout << "PC : " << PC-4 <<endl;
                break;
        }
        cycle_count += 1;

        cycle  = cycle_count ;
        instret  = cycle ;

        if (mstatus.fs==3){
            mstatus.sd = 1;
        }
        else {
            mstatus.sd = 0;
        }

        if (PC >= ((1llu)<<MEM_SIZE)){ //instruction access exception
            mtval = PC;
            PC = excep_function(PC,CAUSE_FETCH_ACCESS,CAUSE_FETCH_ACCESS,CAUSE_FETCH_ACCESS,cp);   
        }

        if (lPC==PC){
            //infinite loop
            cout << "Infinite loop!"<<endl;
            break;
        }
    }

    return 0;
}

//mcycle, ins

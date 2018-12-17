#include <bits/stdc++.h>
#include <math.h>
#include <stdio.h>
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <cstdint>

using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds
using namespace std;

// type defs
enum opcode_t {
				lui    = 0b0110111,
    			auipc  = 0b0010111,
    			jump   = 0b1101111,
    			jumpr  = 0b1100111,
    			cjump  = 0b1100011,
    			load   = 0b0000011,
    			store  = 0b0100011,
                iops   = 0b0010011,
                iops64 = 0b0011011,
                rops   = 0b0110011,
                rops64 = 0b0111011
    		};

typedef __uint64_t uint_t;
typedef __uint64_t data_t;

typedef __uint32_t uint32_t;

#define MEM_SIZE 24
#define XLEN     64
#define FIFO_ADDR_RX 0xe000102c
#define FIFO_ADDR_TX 0xe0001030

#define MASK64 0xFFFFFFFFFFFFFFFF
#define MASK32 0xFFFFFFFF

//#define DEBUG

vector<uint_t> memory(1<<MEM_SIZE); // main memory

vector<uint_t> reg_file(32);       // register file


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
    } else if(s==0 || s==2){
        if (num1==(-pow(2ull,63)) && num2==-1){
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
    } else if(s==0 || s==2){
        if (num1==(-pow(2,31)) && num2==-1){
            switch(s){
                case(0)  : return -pow(2,31); break;
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

void print_reg_file(){
    string reg_file_names [] = {"zero","ra","sp","gp","tp","t0","t1","t2","s0","s1","a0","a1","a2","a3","a4","a5","a6",\
                              "a7","s2","s3","s4","s5","s6","s7","s8","s9","s10","s11","t3","t4","t5","t6"};
    for (int i=0;i<32;i++){
        printf("%s : %lu\n",reg_file_names[i].c_str(),reg_file[i]);
    }
}

int main(){

    ifstream infile("data_hex.txt");
    string line;

    uint_t i = 0;

    uint_t PC = 0;
    uint_t instruction = 0;
    bool branch = false;
    uint_t load_data = 0;
    uint_t store_addr = 0;

    __uint128_t mult_temp;

    //initializing reg file
    reg_file[2]  = 0x40000; //SP
    reg_file[11] = 0x10000  ;

    enum opcode_t opcode;

    while (std::getline(infile, line)) {            // Initialize memory with instructions
        memory[i] = stoul(line,nullptr,16);
        //cout << memory[i] << endl;
        i+=1;
    }
    #ifdef DEBUG
        printf("Mem load successful\n");
    #endif



    while (PC < (1<<MEM_SIZE)){

        #ifdef DEBUG
            sleep_for(milliseconds(500));
            printf("PC : %x\n",PC);
        #endif

        instruction = memory.at(PC/4);

        reg_file[0] = 0;

        #ifdef DEBUG
            bitset<32> ins(instruction);
            cout << "Instruction : "<<ins << endl;
        #endif

        opcode = static_cast<opcode_t>(instruction & 0b1111111);

        uint_t wb_data = 0;

        uint_t rd      = (instruction >> 7 ) & 0b11111   ;
        uint_t func3   = (instruction >> 12) & 0b111     ;
        uint_t rs1     = (instruction >> 15) & 0b11111   ;
        uint_t rs2     = (instruction >> 20) & 0b11111   ;
        uint_t func7   = (instruction >> 25) & 0b1111111 ;

        uint_t imm11_0  = (instruction >> 20) & 0b111111111111 ;
        uint_t imm31_12 = (instruction >> 12) & 1048575 ;       // extract 20 bits

        uint_t imm_j    = (((instruction>>31) & 0b1)<<20) + (instruction & (0b11111111<<12)) + (((instruction>>20) & 0b1)<<11) + (((instruction>>21) & 0b1111111111)<<1); //((instruction>>31) & 0b1)<<20 + (instruction & (0b11111111<<12)) + ((instruction>>20) & 0b1)<<11 +
        uint_t imm_b    = (((instruction>>31) & 0b1)<<12) + (((instruction>>7) & 0b1)<<11) + (((instruction>>25) & 0b111111)<<5) + ((instruction>>7) & 0b11110) ;
        uint_t imm_s    = (((instruction>>25) & 0b1111111)<<5) + ((instruction>>7) & 0b11111) ;
        //printf("IMJ %d\n",imm_j);
        PC += 4;
        switch(opcode){
            case lui :
                #ifdef DEBUG
                    printf("LUI\n");
                #endif
                reg_file[rd] = sign_extend<uint_t>((imm31_12 << 12),32);
                break;

            case auipc :
                #ifdef DEBUG
                    printf("AUIPC\n");
                #endif
                reg_file[rd] = (PC-4) + sign_extend<uint_t>((imm31_12 << 12),32);
                break;

            case jump :
                #ifdef DEBUG
                    printf("JUMP\n");
                #endif
                wb_data = PC;
                PC = (PC-4) + sign_extend<uint_t>(imm_j,21); //21 bits sign extend
                reg_file[rd] = wb_data ;// PC + 4 to rd
                break;

            case jumpr :
                #ifdef DEBUG
                    printf("JUMPR\n");
                #endif
                wb_data = PC;
                PC = (reg_file[rs1] + sign_extend<uint_t>(imm11_0,12)) & 0b0; //setting LSB to 0 as spec page 20
                reg_file[rd] = wb_data ;// PC + 4 to rd
                break;

            case cjump :
                #ifdef DEBUG
                    printf("CJUMP\n");
                #endif
                switch(func3){
                    case 0b000 : branch = (reg_file[rs1] == reg_file[rs2]); break; //BEQ

                    case 0b001 : branch = (reg_file[rs1] != reg_file[rs2]); break; //BNE

                    case 0b100 : branch = (signed_value(reg_file[rs1]) < signed_value(reg_file[rs2])); break; //BLT

                    case 0b101 : branch = (signed_value(reg_file[rs1]) > signed_value(reg_file[rs2])); break; //BGE

                    case 0b110 : branch = (reg_file[rs1] < reg_file[rs2]); break; //BLTU

                    case 0b111 : branch = (reg_file[rs1] > reg_file[rs2]); break; //BGEU

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
                if (((reg_file[rs1] + sign_extend<uint_t>(imm11_0,12)) != FIFO_ADDR_RX) && (((reg_file[rs1] + sign_extend<uint_t>(imm11_0,12)) != FIFO_ADDR_TX))){
                load_data = memory.at(reg_file[rs1] + sign_extend<uint_t>(imm11_0,12));
                        switch(func3){
                            case 0b000 : reg_file[rd] = sign_extend<uint_t>(load_data & 0xFF      , 8); break; //LB sign extend  8 bit value

                            case 0b001 : reg_file[rd] = sign_extend<uint_t>(load_data & 0xFFFF    ,16); break; //LH sign extend 16 bit value

                            case 0b010 : reg_file[rd] = sign_extend<uint_t>(load_data & 0xFFFFFFFF,32); break; //LW sign extend 32 bit value

                            case 0b100 : reg_file[rd] = load_data & 0xFF      ; break; //LBU zero extend  8 bit value

                            case 0b101 : reg_file[rd] = load_data & 0xFFFF    ; break; //LHU zero extend 16 bit value

                            case 0b110 : reg_file[rd] = load_data & 0xFFFFFFFF; break; //LWU zero extend 32 bit value

                            case 0b011 : reg_file[rd] = load_data ; break;//LD

                            default : printf("******INVALID INSTRUCTION******\nINS :%lu\nOPCODE :%lu\n",instruction,instruction & 0b1111111);
                                bitset<3> ins(func3);
                                cout<<  "func3 : "<<ins<<endl;
                                break;
                        }
                } else if ((reg_file[rs1] + sign_extend<uint_t>(imm11_0,12)) == FIFO_ADDR_RX) {
                    reg_file[rd] = 0 ;
                }
                else if ((reg_file[rs1] + sign_extend<uint_t>(imm11_0,12)) == FIFO_ADDR_TX){
                    reg_file[rd] = (uint_t)getchar() ;
                }
                break;

            case store :
                #ifdef DEBUG
                    printf("STORE\n");
                #endif
                store_addr = reg_file[rs1] + sign_extend<uint_t>(imm_s,12);
                if (store_addr != FIFO_ADDR_TX){
                    switch(func3){                                                      // Setting lower n bits to 0 and adding storing value
                        case 0b000 : memory.at(store_addr/4) = (memory.at(store_addr/4) & (0xFFFFFFFFFFFFFF<< 8)) + (reg_file[rs2] & 0xFF      )    ; break;//SB  setting LSB 8 bit

                        case 0b001 : memory.at(store_addr/4) = (memory.at(store_addr/4) & (0xFFFFFFFFFFFF  <<16)) + (reg_file[rs2] & 0xFFFF    )    ; break;//SH setting LSB 16 bit value

                        case 0b010 : memory.at(store_addr/4) = ((memory.at(store_addr/4) & (0xFFFFFFFFull   <<32))) + (reg_file[rs2] & 0xFFFFFFFF)    ; break;//SW setting LSB 32 bit value

                        case 0b011 : memory.at(store_addr/4) = reg_file[rs2] ; break; //SD

                        default : printf("******INVALID INSTRUCTION******\nINS :%lu\nOPCODE :%lu\n",instruction,instruction & 0b1111111);
                        bitset<3> ins(func3);
                        cout<<  "func3 : "<<func3<<endl;
                        break;
                    }
                } else {
                    #ifdef DEBUG
                        printf("STORE2\n");
                    #endif
                        printf("STORE2 %llu\n",rs2);
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
                        wb_data = reg_file[rs1] << (imm11_0 & 0b11111); //SLLI
                        break;

                    case 0b101 :
                        if ((imm11_0 >> 5) == 0)
                            wb_data = reg_file[rs1] >> (imm11_0 & 0b11111); //SRLI
                        else if ((imm11_0 >> 5) == 1)
                            wb_data = (reg_file[rs1] & (1llu<<63)) | (reg_file[rs1] >> (imm11_0 & 0b11111)); //SRAI
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
                case 0b000 :  //ADDIW
                        wb_data = sign_extend<uint_t>(MASK32 & (reg_file[rs1] + sign_extend<uint_t>(imm11_0,12)),32);
                        break;

                    case 0b001 : //SLLIW
                        wb_data = sign_extend<uint_t>(MASK32 & (reg_file[rs1] << (imm11_0 & 0b11111)),32);
                        break;

                    case 0b101 :
                        if ((imm11_0 >> 5) == 0) //SRLIW
                            wb_data = sign_extend<uint_t>(MASK32 & (reg_file[rs1] >> (imm11_0 & 0b11111)),32);
                        else if ((imm11_0 >> 5) == 1) //SRAIW
                            wb_data = sign_extend<uint_t>(MASK32 & ((reg_file[rs1] & (1lu<<31)) | (reg_file[rs1] >> (imm11_0 & 0b11111))),32);
                        break;


            case rops :
                if (func7 == 0b0000001){
                    #ifdef DEBUG
                        printf("ROPS\n");
                    #endif
                    switch (func3) {
                        case 0b000 : //MUL
                            mult_temp = reg_file[rs1] * reg_file[rs2];
                            reg_file[rd] = mult_temp & MASK64;
                            break;

                        case 0b001 : //MULH
                            mult_temp = (__uint128_t)(signed_value(reg_file[rs1]) * signed_value(reg_file[rs2]));
                            reg_file[rd] = (mult_temp >> 64) & MASK64;
                            break;

                        case 0b010 : //MULHSU
                            mult_temp = (__uint128_t)(signed_value(reg_file[rs1]) * reg_file[rs2]);
                            reg_file[rd] = (mult_temp >> 64) & MASK64;
                            break;

                        case 0b011 : //MULHU
                            mult_temp = reg_file[rs1] * reg_file[rs2];
                            reg_file[rd] = (mult_temp >> 64) & MASK64;
                            break;

                        case 0b100 : //DIV
                            reg_file[rd] = (uint_t)divi<int64_t>(signed_value(reg_file[rs1]), signed_value(reg_file[rs2]),0);
                        case 0b101 : //DIVU
                            reg_file[rd] = divi<uint_t>(reg_file[rs1], reg_file[rs2],1);
                        case 0b110 : //REM
                            reg_file[rd] = (uint_t)divi<int64_t>(signed_value(reg_file[rs1]), signed_value(reg_file[rs2]),2);
                        case 0b111 : //REMU
                            reg_file[rd] = divi<uint_t>(reg_file[rs1], reg_file[rs2],3);
                    }
                }
                break;

            case rops64 :
                if (func7 == 0b0000001){
                    #ifdef DEBUG
                        printf("ROPS64\n");
                    #endif
                    switch (func3) {
                        case 0b000 : //MULW
                            reg_file[rd] = sign_extend<uint_t>(((reg_file[rs1] & MASK32) * (reg_file[rs2] & MASK32)) & MASK32, 32) ;
                            break;
                        case 0b100 : //DIVW
                            reg_file[rd] = sign_extend((uint_t)divi32<int32_t>(signed_value32(reg_file[rs1]), signed_value32(reg_file[rs2]),0),32);
                        case 0b101 : //DIVUW
                            reg_file[rd] = sign_extend((uint_t)divi32<uint32_t>(reg_file[rs1], reg_file[rs2],1),32);
                        case 0b110 : //REMW
                            reg_file[rd] = sign_extend((uint_t)divi32<int32_t>(signed_value32(reg_file[rs1]), signed_value32(reg_file[rs2]),2),32);
                        case 0b111 : //REMUW
                            reg_file[rd] = sign_extend((uint_t)divi32<uint32_t>(reg_file[rs1], reg_file[rs2],3),32);
                    }
                }
                break;
            default :
                printf("default\n");
                bitset<32> ins1(instruction);
                cout << "Instruction : "<<ins1 << endl;
                printf("PC : %x\n",PC-4);
        }
    }

	return 0;
}


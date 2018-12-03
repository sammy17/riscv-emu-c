#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <bits/stdc++.h> 

using namespace std;

// type defs
enum opcode_t { 
				  lui   = 0b0110111,
    			auipc = 0b0010111,
    			jump  = 0b1101111,
    			jumpr = 0b1100111,
    			cjump = 0b1100011,
    			load  = 0b0000011,
    			store = 0b0100011
    		};

typedef uint64_t uint_t;
typedef uint64_t data_t;

#define MEM_SIZE 10
#define XLEN     64
#define FIFO_ADDR 0xe0001030

vector<uint_t> memory(1<<MEM_SIZE); // main memory

vector<uint_t> reg_file(32);       // register file


template<class T>
T sign_extend(T x, const int bits) {
    T m = 1;
    m <<= bits - 1;
    return (x ^ m) - m;
}

int64_t signed_value(uint_t x){
  if (((x>>63) & 0b1) == 1)
      return (x ^ (1llu<<63)) - (1llu<<63);
  else
      return x;
}
/*enum opcode_t { 
				0b0110111 = lui   ,
    			0b0010111 = auipc ,
    			0b1101111 = jump  ,
    			0b1100111 = jumpr ,
    			0b1100011 = cjump ,
    			0b0000011 = load  ,
    			0b0100011 = store

        try {
        } catch (const out_of_range& e){
            printf("Memory out of range at PC : %lu\n",PC);
            return 1;
        }
    		};*/



int main(){

    ifstream infile("data_hex.txt");
    string line;

    uint_t i = 0;

    uint_t PC = 0;
    uint_t instruction = 0;
    bool branch = false;
    uint_t load_data = 0;
    uint_t store_addr = 0;

    enum opcode_t opcode;

    while (std::getline(infile, line)) {            // Initialize memory with instructions
        memory[i] = stoul(line,nullptr,16);
        //cout << memory[i] << endl;
        i+=1;
    }
    printf("Mem load successful\n");



    while (PC < (1<<MEM_SIZE)){

        instruction = memory.at(PC/4);

        reg_file[0] = 0;
        //bitset<32> ins(instruction);

        //cout << instruction << endl;

        opcode = static_cast<opcode_t>(instruction & 0b1111111);

        uint_t wb_data = 0;

        uint_t rd      = (instruction >> 7 ) & 0b11111   ;
        uint_t func3   = (instruction >> 12) & 0b111     ;
        uint_t rs1     = (instruction >> 15) & 0b11111   ;
        uint_t rs2     = (instruction >> 20) & 0b11111   ;
        uint_t func7   = (instruction >> 25) & 0b1111111 ;

        uint_t imm11_0  = (instruction >> 20) & 0b111111111111 ;
        uint_t imm31_12 = (instruction >> 12) & 1048575 ;       // extract 20 bits

        uint_t imm_j    = ((instruction>>31) & 0b1)<<20 + (((rs1<<3)+func3)<<12) + ((instruction>>20) & 0b1)<<11 + ((instruction>>20) & 0b11111111110) ;
        uint_t imm_b    = ((instruction>>31) & 0b1)<<12 + ((instruction>>7) & 0b1)<<11 + ((instruction>>25) & 0b111111)<<5 + ((instruction>>7) & 0b11110) ;
        uint_t imm_s    = ((instruction>>25) & 0b1111111)<<5 + (instruction>>7) & 0b11111 ;

        PC += 4;
        switch(opcode){
            case lui : 
                printf("LUI\n");
                reg_file[rd] = sign_extend<uint_t>((imm31_12 << 12),32);
                break;

            case auipc : 
                printf("AUIPC\n");
                reg_file[rd] = (PC-4) + sign_extend<uint_t>((imm31_12 << 12),32);
                break;

            case jump : 
                printf("JUMP\n");
                wb_data = PC;
                PC = (PC-4) + sign_extend<uint_t>(imm_j,21); //21 bits sign extend
                reg_file[rd] = wb_data ;// PC + 4 to rd
                break;

            case jumpr : 
                printf("JUMPR\n");
                wb_data = PC;
                PC = (reg_file[rs1] + sign_extend<uint_t>(imm11_0,12)) & 0b0; //setting LSB to 0 as spec page 20
                reg_file[rd] = wb_data ;// PC + 4 to rd
                break;

            case cjump : 
                printf("CJUMP\n");
                switch(func3){
                    case 0b000 : branch = (reg_file[rs1] == reg_file[rs2]); //BEQ

                    case 0b001 : branch = (reg_file[rs1] != reg_file[rs2]); //BNE

                    case 0b100 : branch = (signed_value(reg_file[rs1]) < signed_value(reg_file[rs2])); //BLT

                    case 0b101 : branch = (signed_value(reg_file[rs1]) > signed_value(reg_file[rs2])); //BGE

                    case 0b110 : branch = (reg_file[rs1] < reg_file[rs2]); //BLTU

                    case 0b111 : branch = (reg_file[rs1] > reg_file[rs2]); //BGEU
                }
                if (branch==true)
                    PC = PC - 4 + sign_extend<uint_t>(imm_b,13);

                break;

            case load : 
                printf("LOAD\n");
                load_data = memory.at(reg_file[rs1] + sign_extend<uint_t>(imm11_0,12));
                switch(func3){
                    case 0b000 : reg_file[rd] = sign_extend<uint_t>(load_data & 0xFF      , 8); //LB sign extend  8 bit value

                    case 0b001 : reg_file[rd] = sign_extend<uint_t>(load_data & 0xFFFF    ,16); //LH sign extend 16 bit value

                    case 0b010 : reg_file[rd] = sign_extend<uint_t>(load_data & 0xFFFFFFFF,32); //LW sign extend 32 bit value

                    case 0b100 : reg_file[rd] = load_data & 0xFF      ; //LBU zero extend  8 bit value

                    case 0b101 : reg_file[rd] = load_data & 0xFFFF    ; //LHU zero extend 16 bit value

                    case 0b110 : reg_file[rd] = load_data & 0xFFFFFFFF; //LWU zero extend 32 bit value

                    case 0b011 : reg_file[rd] = load_data ; //LD
                }
                break;

            case store : 
                printf("STORE\n");
                store_addr = memory.at(reg_file[rs1] + sign_extend<uint_t>(imm_s,12));
                if (store_addr != FIFO_ADDR){
                    printf("STORE1\n");
                    cout << sign_extend<uint_t>(imm_s,12) <<endl;
                    store_addr = memory.at(reg_file[rs1] + sign_extend<uint_t>(imm_s,12));
                    switch(func3){                                                      // Setting lower n bits to 0 and adding storing value
                        case 0b000 : memory.at(store_addr/4) = (memory.at(store_addr/4) & (0xFFFFFFFFFFFFFF<< 8)) + (reg_file[rs2] & 0xFF      )    ; //SB  setting LSB 8 bit 

                        case 0b001 : memory.at(store_addr/4) = (memory.at(store_addr/4) & (0xFFFFFFFFFFFF  <<16)) + (reg_file[rs2] & 0xFFFF    )    ; //SH setting LSB 16 bit value

                        case 0b010 : memory.at(store_addr/4) = (memory.at(store_addr/4) & (0xFFFFFFFFull   <<32)) + (reg_file[rs2] & 0xFFFFFFFF)    ; //SW setting LSB 32 bit value

                        case 0b011 : memory.at(store_addr/4) = reg_file[rs2] ; //SD
                    }
                } else {
                    printf("STORE2\n");
                    cout << (char)reg_file[rs2] ;
                }
                break;                
            default : printf("default\n");
        }
    }

	return 0;
}


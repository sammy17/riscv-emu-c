#ifndef _EMU_H_
#define _EMU_H_

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

using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds
using namespace std;

typedef uint64_t uint_t;
typedef uint64_t data_t;

#define MEM_SIZE 25
#define XLEN     64
#define FIFO_ADDR_RX 0xe000102c
#define FIFO_ADDR_TX 0xe0001030

#define MASK64 0xFFFFFFFFFFFFFFFFllu
#define MASK32 0xFFFFFFFFllu


#define CAUSE_MISALIGNED_FETCH 0x0
#define CAUSE_FETCH_ACCESS 0x1
#define CAUSE_ILLEGAL_INSTRUCTION 0x2
#define CAUSE_BREAKPOINT 0x3
#define CAUSE_MISALIGNED_LOAD 0x4
#define CAUSE_LOAD_ACCESS 0x5
#define CAUSE_MISALIGNED_STORE 0x6
#define CAUSE_STORE_ACCESS 0x7
#define CAUSE_USER_ECALL 0x8
#define CAUSE_SUPERVISOR_ECALL 0x9
#define CAUSE_HYPERVISOR_ECALL 0xa
#define CAUSE_MACHINE_ECALL 0xb
#define CAUSE_FETCH_PAGE_FAULT 0xc
#define CAUSE_LOAD_PAGE_FAULT 0xd
#define CAUSE_STORE_PAGE_FAULT 0xf

vector<uint_t> memory(1<<MEM_SIZE); // main memory

vector<uint_t> reg_file(32);       // register file

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
                rops64 = 0b0111011,
                amo    = 0b0101111,
                fence  = 0b0001111,
                systm = 0b1110011
            };

enum plevel_t {
    MMODE = 0b11,
    HMODE = 0b10,
    SMODE = 0b01,
    UMODE = 0b00
};







uint_t store_word(uint_t store_addr, uint_t load_data, uint_t value){
    uint_t wb_data = 0;
    if ((store_addr%8)==0){
        wb_data = (load_data & ((0xFFFFFFFFull)   <<32)) + (value & 0xFFFFFFFF);
    }
    else if ((store_addr%8)==4){
        wb_data = (load_data & (0xFFFFFFFF)) + ((value & 0xFFFFFFFFull)<<32);
    }
    else{
        //cout<<"######## Mis-aligned memory store #########\n";
        wb_data = -1;
    }

    return wb_data;
}
uint_t store_halfw(uint_t store_addr, uint_t load_data, uint_t value){
    uint_t wb_data = 0;
    switch(store_addr%8){
        case 0 :
            wb_data = ( load_data & ((0xFFFFFFFFFFFFull)<<16)) + (value & 0xFFFF);
            break;
        case 2 :
            wb_data = ( load_data & ((0xFFFFFFFFull)<<32)) + ((value & 0xFFFF)<<16) + (load_data & 0xFFFF);
            break;
        case 4 :
            wb_data = ( load_data & ((0xFFFFull)<<48)) + ((value & 0xFFFF)<<32) + (load_data & 0xFFFFFFFF);
            break;
        case 6 :
            wb_data = ((value & 0xFFFF)<<48) + (load_data & 0xFFFFFFFFFFFF);
            break;
        default :
            //cout<<"######## Mis-aligned memory store #########\n";
            wb_data = -1;
            break;
    }
    return wb_data;
}

uint_t store_byte(uint_t store_addr, uint_t load_data, uint_t value){
    uint_t wb_data = 0;
    switch(store_addr%8){
        case 0 :
            wb_data = ( load_data & ((0xFFFFFFFFFFFFFFull)<<8)) + (value & 0xFF);
            break;
        case 1 :
            wb_data = ( load_data & ((0xFFFFFFFFFFFFull)<<16)) + ((value & 0xFF)<<8) + (load_data & 0xFF);
            break;
        case 2 :
            wb_data = ( load_data & ((0xFFFFFFFFFFull)<<24)) + ((value & 0xFF)<<16) + (load_data & 0xFFFF);
            break;
        case 3 :
            wb_data = ( load_data & ((0xFFFFFFFFull)<<32)) + ((value & 0xFF)<<24) + (load_data & 0xFFFFFF);
            break;
        case 4 :
            wb_data = ( load_data & ((0xFFFFFFull)<<40)) + ((value & 0xFF)<<32) + (load_data & 0xFFFFFFFF);
            break;
        case 5 :
            wb_data = ( load_data & ((0xFFFFull)<<48)) + ((value & 0xFF)<<40) + (load_data & 0xFFFFFFFFFF);
            break;
        case 6 :
            wb_data = ( load_data & ((0xFFull)<<56)) + ((value & 0xFF)<<48) + (load_data & 0xFFFFFFFFFFFF);
            break;
        case 7 :
            wb_data = ((value & 0xFF)<<56) + (load_data & 0xFFFFFFFFFFFFFF);
            break;
        default :
            //cout<<"######## Mis-aligned memory store #########\n";
            wb_data = -1;
            break;
    }
    return wb_data;
}

uint_t load_word(uint_t load_addr, uint_t load_data){
    uint_t wb_data = 0;
    if ((load_addr%8)==0)
        wb_data = (load_data & 0xFFFFFFFF);
    else if ((load_addr%8)==4)
        wb_data = (((load_data)>>32) & 0xFFFFFFFF);
    else{
        //cout<< "Mis-aligned memory load\n";
        wb_data = -1;
    }

    return wb_data;
}

uint_t load_halfw(uint_t load_addr, uint_t load_data){
    uint_t wb_data = 0;
    switch(load_addr%8){
        case 0 :
            wb_data = (load_data & 0xFFFF) ;
            break;
        case 2 :
            wb_data = (((load_data)>>16) & 0xFFFF) ;
            break;
        case 4 :
            wb_data = (((load_data)>>32) & 0xFFFF) ;
            break;
        case 6 :
            wb_data = (((load_data)>>48) & 0xFFFF) ;
            break;
        default :
            //cout<<"######## Mis-aligned memory load #########\n";
            wb_data = -1;
            break;
    }
    return wb_data;
}


uint_t load_byte(uint_t load_addr, uint_t load_data){
    uint_t wb_data = 0;
    switch(load_addr%8){
        case 0 :
            wb_data = (load_data & 0xFF) ;
            break;
        case 1 :
            wb_data = (((load_data)>>8) & 0xFF) ;
            break;
        case 2 :
            wb_data = (((load_data)>>16) & 0xFF) ;
            break;
        case 3 :
            wb_data = (((load_data)>>24) & 0xFF) ;
            break;
        case 4 :
            wb_data = (((load_data)>>32) & 0xFF) ;
            break;
        case 5 :
            wb_data = (((load_data)>>40) & 0xFF) ;
            break;
        case 6 :
            wb_data = (((load_data)>>48) & 0xFF) ;
            break;
        case 7 :
            wb_data = (((load_data)>>56) & 0xFF) ;
            break;
        default :
            //cout<<"######## Mis-aligned memory load #########\n";
            wb_data = -1;
            break;
    }
    return wb_data;
}

#endif
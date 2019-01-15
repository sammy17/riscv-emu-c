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

#define ECODE_M_ECALL 11
#define ECODE_S_ECALL 9
#define ECODE_U_ECALL 8

#define ECODE_M_EBREAK 3


typedef uint64_t uint_t;
typedef uint64_t data_t;

#define MEM_SIZE 24
#define XLEN     64
#define FIFO_ADDR_RX 0xe000102c
#define FIFO_ADDR_TX 0xe0001030

#define MASK64 0xFFFFFFFFFFFFFFFFllu
#define MASK32 0xFFFFFFFFllu

#endif
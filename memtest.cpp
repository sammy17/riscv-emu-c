#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <bits/stdc++.h> 
#include <math.h> 
#include <chrono>
#include <thread>

#include "emu.h"
#include "csr_file.h"

using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds
using namespace std;


//#define DEBUG

vector<uint_t> memory(1<<MEM_SIZE); // main memory

vector<uint_t> reg_file(32);       // register file

uint_t getINST(uint_t PC,vector<uint_t> * memory){
	if(PC%2==0)
		return ((MASK32) & (memory->at(PC/2)));
	else
		return ((memory->at(PC/2))>>32);
}

uint_t store_word(uint_t store_addr, uint_t load_data, uint_t value){
	uint_t wb_data = 0;
	if ((store_addr%8)==0){
		wb_data = (load_data & ((0xFFFFFFFFull)   <<32)) + (value & 0xFFFFFFFF);
	}
	else if ((store_addr%8)==4){
		wb_data = (load_data & (0xFFFFFFFF)) + ((value & 0xFFFFFFFFull)<<32);
	}
	else{
		cout<<"######## Mis-aligned memory store #########\n";
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
			cout<<"######## Mis-aligned memory store #########\n";
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
			cout<<"######## Mis-aligned memory store #########\n";
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
		cout<< "Mis-aligned memory load\n";
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
			cout<<"######## Mis-aligned memory load #########\n";
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
			cout<<"######## Mis-aligned memory load #########\n";
			wb_data = -1;
			break;
	}
	return wb_data;		
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

int64_t signed_value(uint_t x){
  if (((x>>63) & 0b1) == 1)
      return (x ^ (1llu<<63)) - (1llu<<63);
  else
      return x;
}

struct mstat{
	uint8_t uie, sie, mie, upie, spie, mpie, spp, mpp, fs, xs, mprv, sum, mxr, tvm, tw, tsr, uxl, sxl, sd;
	mstat() {
		uie = 0; sie = 0; mie = 0;
		upie = 0; spie = 0; mpie = 0; 
		spp = 0; mpp = 0b11; 
		fs = 0; xs = 0; mprv = 0; sum = 0; mxr = 0; tvm = 0; tw = 0; tsr = 0; uxl = 0; sxl = 0; sd = 0; 
	}
	uint_t concat_reg(){
		return (((uint_t)sd<<63)+((uint_t)sxl<<34)+((uint_t)uxl<<32)+(tsr<<22)+(tw<<21)+(tvm<<20)+(mxr<<19)+(sum<<18)+(mprv<<17)+(xs<<15)+(fs<<13)+(mpp<<11)+(spp<<8)+(mpie<<7)+(spie<<5)+(upie<<4)+(mie<<3)+(sie<<1)+uie);
	}

	void write_reg(uint_t val){
		uie = (val & 0b1); sie = ((val>>1)& 0b1); mie = ((val>>3)& 0b1);
		upie= ((val>>5)& 0b1); spie= ((val>>6)& 0b1); mpie= ((val>>7)& 0b1); 
		spp= ((val>>8)& 0b1); mpp = ((val>>11)& 0b11);
		fs= ((val>>13)& 0b11); xs= ((val>>15)& 0b11); mprv= ((val>>17)& 0b1); sum= ((val>>18)& 0b1); mxr= ((val>>19)& 0b1); tvm= ((val>>20)& 0b1); tw= ((val>>21)& 0b1); tsr= ((val>>22)& 0b1); uxl= ((val>>32)& 0b11); sxl= ((val>>34)& 0b11); sd= ((val>>63)& 0b1); 
	}
} mstatus;


uint_t csr_read(uint_t csr_addr){
	switch(csr_addr){
		case MSTATUS :
			return mstatus.concat_reg();
			break;
		default:
			cout << "CSR not implemented : " << csr_addr << endl;
			break;
	}
}

void csr_write(uint_t csr_addr, uint_t val){
	switch(csr_addr){
		case MSTATUS :
			mstatus.write_reg(val);
			break;
		default:
			cout << "CSR not implemented : " << csr_addr << endl;
			break;
	}
}

int main(){

    ifstream infile("data_test.txt");
    string line;

    //printf("1\n");

    uint_t i = 0;

    uint_t PC = 0;
    uint_t instruction = 0;

    //uint32_t* mem32_ptr ;
    //printf("2\n");
/*    uint_t temp;

    while (std::getline(infile, line)) {            // Initialize memory with instructions
    	//printf("*****************************\n");
    	temp = stoul(line,nullptr,16);
    	if (i%2==0)
        	memory.at(i/2) = temp;
        else
        	memory.at(i/2) = ((memory.at(i/2)) | ((temp)<<32));
        //printf("%llx\n",((temp)<<32));
        //printf("3\n");
        //cout << memory[i] << endl;
        //printf("Value : %x\n",stoul(line,nullptr,16));
        //printf("L 64 %lu : %lx\n",i,memory[i/2] & MASK32 );
        //printf("H 64 %lu : %lx\n",i,memory[i/2]);
        i+=1;
    }


    for (int j=0;j<20;j++){
    	printf("Hello %X\n",getINST(j,&memory));
    }
    //printf("4\n");



*/
    /*
    cout << hex<<store_word(8,0xFFFFFFFFFFFFFFFFlu,0)<<endl;
    cout << hex<<store_word(4,0xFFFFFFFFFFFFFFFFlu,0)<<endl;
    cout << hex<<store_word(3,0xFFFFFFFFFFFFFFFFlu,0)<<endl;
    cout << "###################\n";
    cout << hex<<store_halfw(8,0xFFFFFFFFFFFFFFFFlu,0)<<endl;
    cout << hex<<store_halfw(2,0xFFFFFFFFFFFFFFFFlu,0)<<endl;
    cout << hex<<store_halfw(4,0xFFFFFFFFFFFFFFFFlu,0)<<endl;
    cout << hex<<store_halfw(6,0xFFFFFFFFFFFFFFFFlu,0)<<endl;
    cout << hex<<store_halfw(1,0xFFFFFFFFFFFFFFFFlu,0)<<endl;
    cout << "###################\n";
    cout << hex<<store_byte(8,0xFFFFFFFFFFFFFFFFlu,0)<<endl;
    cout << hex<<store_byte(1,0xFFFFFFFFFFFFFFFFlu,0)<<endl;
    cout << hex<<store_byte(2,0xFFFFFFFFFFFFFFFFlu,0)<<endl;
    cout << hex<<store_byte(3,0xFFFFFFFFFFFFFFFFlu,0)<<endl;
    cout << hex<<store_byte(4,0xFFFFFFFFFFFFFFFFlu,0)<<endl;
    cout << hex<<store_byte(5,0xFFFFFFFFFFFFFFFFlu,0)<<endl;
    cout << hex<<store_byte(6,0xFFFFFFFFFFFFFFFFlu,0)<<endl;
    cout << hex<<store_byte(7,0xFFFFFFFFFFFFFFFFlu,0)<<endl;
*//*
    cout << hex<<load_word(8,0xfedcba9876543210lu)<<endl;
    cout << hex<<load_word(4,0xfedcba9876543210lu)<<endl;
    cout << hex<<load_word(2,0xfedcba9876543210lu)<<endl;
    cout << "###################\n";
    cout << hex<<load_halfw(8,0xfedcba9876543210lu)<<endl;
    cout << hex<<load_halfw(2,0xfedcba9876543210lu)<<endl;
    cout << hex<<load_halfw(4,0xfedcba9876543210lu)<<endl;
    cout << hex<<load_halfw(6,0xfedcba9876543210lu)<<endl;
    cout << hex<<load_halfw(7,0xfedcba9876543210lu)<<endl;
    cout << "###################\n";
    cout << hex<<load_byte(8,0xfedcba9876543210lu)<<endl;
    cout << hex<<load_byte(1,0xfedcba9876543210lu)<<endl;
    cout << hex<<load_byte(2,0xfedcba9876543210lu)<<endl;
    cout << hex<<load_byte(3,0xfedcba9876543210lu)<<endl;
    cout << hex<<load_byte(4,0xfedcba9876543210lu)<<endl;
    cout << hex<<load_byte(5,0xfedcba9876543210lu)<<endl;
    cout << hex<<load_byte(6,0xfedcba9876543210lu)<<endl;
    cout << hex<<load_byte(7,0xfedcba9876543210lu)<<endl;
*/
    
    //cout << hex<<signed_value(1ull<<63)<<endl;
    //cout << divi<int64_t>(100,10,0)<<endl;
/*
    uint_t val = (uint_t)(-8);

    bitset<64> ins1(val);

    uint_t wb_data = (( val & ((1llu)<<63)) | ((val) >> (2 & 0b111111)));
    //cout << hex << wb_data << endl;

    bitset<64> ins2(wb_data);

    cout << ins1 << endl;
    cout << ins2 << endl;
    cout << signed_value(wb_data) << endl;
*/
    //mstat mstatus;

    cout << mstatus.concat_reg() <<endl;

    mstatus.mie = 1;

    cout << mstatus.concat_reg() <<endl;

    mstatus.write_reg(0b1000000001000);

    cout << mstatus.concat_reg() <<endl;
    cout << mstatus.mpp <<endl;

    cout << csr_read(MSTATUS) <<endl;

    csr_write(MSTATUS, 0b0100000001000);

    cout << csr_read(MSTATUS) <<endl;

    //cout << mstatus <<endl;


/*
    while (std::getline(infile, line)) {            // Initialize memory with instructions
        memory[i] = stoul(line,nullptr,16);
        //cout << memory[i] << endl;
        i+=1;
    }

*/

    return 0;

}
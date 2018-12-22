#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <bits/stdc++.h> 
#include <math.h> 
#include <chrono>
#include <thread>

using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds
using namespace std;


typedef uint64_t uint_t;
typedef uint64_t data_t;

#define MEM_SIZE 24
#define XLEN     64
#define FIFO_ADDR_RX 0xe000102c
#define FIFO_ADDR_TX 0xe0001030

#define MASK64 0xFFFFFFFFFFFFFFFF
#define MASK32 0xFFFFFFFF

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
*/
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

/*
    while (std::getline(infile, line)) {            // Initialize memory with instructions
        memory[i] = stoul(line,nullptr,16);
        //cout << memory[i] << endl;
        i+=1;
    }

*/

    return 0;

}
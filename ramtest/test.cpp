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
#include "data_hex.h"

using namespace std;


uint8_t memory[20];

uint16_t read_halfw(uint64_t addr, uint8_t* memory){
	return *(uint16_t*)(memory+addr);
}

void write_halfw(uint64_t addr, uint8_t* memory, uint16_t value){
	*(uint16_t*)(memory+addr) = value;
}

uint32_t read_word(uint64_t addr, uint8_t* memory){
	return *(uint32_t*)(memory+addr);
}

void write_word(uint64_t addr, uint8_t* memory, uint32_t value){
	*(uint32_t*)(memory+addr) = value;
}

uint64_t read_double(uint64_t addr, uint8_t* memory){
	return *(uint64_t*)(memory+addr);
}

void write_double(uint64_t addr, uint8_t* memory, uint64_t value){
	*(uint64_t*)(memory+addr) = value;
}

int main(){

	memset(memory, 0, 20);

	memory[0] = 255;

	memory[1] = 1;

	memory[2] = 128;

	write_double((uint64_t)8, memory, (uint64_t)4567);


	//uint64_t &mtime = (uint64_t*)(&memory;

	cout << "value : "<< read_double((uint64_t)8, memory)<<endl;

	//mtime = 1234;

	cout << "value : "<< read_double((uint64_t)8, memory)<<endl;

	//uint16_t* ptr_32t ;
	//ptr_32t = (uint16_t*) memory;


	//cout << "value : "<< *ptr_32t <<endl;

	//*ptr_32t = 255;

	//cout << "value : "<< *ptr_32t <<endl;

	cout << "value : "<< read_halfw((uint64_t)0, memory) <<endl;

	cout << "value : "<< read_halfw((uint64_t)2, memory) <<endl;

	write_halfw((uint64_t)2, memory, 520);

	cout << "value : "<< read_halfw((uint64_t)2, memory) <<endl;

	return 0;
}
#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <bits/stdc++.h> 

using namespace std;
typedef uint64_t uint_t;
typedef uint64_t data_t;


template<class T>
T sign_extend(T x, const int bits) {
    T m = 1;
    m <<= bits - 1;
    return (x ^ m) - m;
}

int8_t signed_value(uint8_t x){
  if (((x>>7) & 0b1) == 1)
      return x - (1<<8);
  else
      return x;
}

__uint128_t testt;

int main(){
	uint8_t a = 255;

  ifstream infile("dtb");
    string line;

    uint_t temp;

    char val;


    //infile.read(&val,sizeof(uint8_t));  // read 1 byte from the file

    while(infile.read(&val,sizeof(uint8_t))){
    cout << hex << (0xFF & (uint32_t)int(val));  // print it as 2-digit hex 
    infile.read(&val,sizeof(uint8_t)); 
  }

    //while (std::getline(infile, line)) {            // Initialize memory with instructions
    //    temp = stoul(line,nullptr,16);
    //    cout << hex << temp <<endl;
    //}
	//uint32_t b = 
    /*bitset<8> ins(a & (0xF<<4));

    cout << ins << endl;

    cout << (char)127 << endl;

    cout << (char)25 << endl;

    uint8_t b = sign_extend<uint8_t>(a & 0b1111,4);

    bitset<8> b1(b);

    cout << b1 << endl;

    uint8_t c = 0b10110100;

    bitset<8> c1(signed_value(c));
    bitset<8> c2(signed_value(c)>>1);
    cout << c1 <<endl;
    cout << c2 <<endl;*/

    //printf("Value : %d\n",signed_value(128));

	return 0;
}
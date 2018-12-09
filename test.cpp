#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <bits/stdc++.h> 

using namespace std;

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

int main(){
	uint8_t a = 255;
	//uint32_t b = 
    bitset<8> ins(a & (0xF<<4));

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
    cout << c2 <<endl;

    //printf("Value : %d\n",signed_value(128));

	return 0;
}
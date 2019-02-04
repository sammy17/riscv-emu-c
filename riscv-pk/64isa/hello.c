#include <stdint.h>
#include <stdarg.h>
#include "mtrap.h"
//#include <stdio.h>

extern void printm(const char* s, ...);

//#define hprintf printm



char* hscanf();
void hprintf(const char *format, ...);

int main()
{		
	float y,z;
	//float z;
		int x;
		y=1.1234;
		z=20.1;
		//x = 21310;
		x= z*y*1000;

		printm("hello_world  %d\n",x);

		while (1);
}



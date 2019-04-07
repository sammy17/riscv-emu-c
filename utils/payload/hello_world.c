#include <stdint.h>
#include <stdarg.h>
//#include <stdio.h>
void hprintf(const char *format, ...);
//void hprintf_c(int c);

int main()
{		
		//hprintf_c((int)'o');
	hprintf("Hello_world\n");

	while(1);
}



void hprintf_c(int c){
	// volatile char* serial_base = (char*) OUTPORT;
	// *serial_base = c;

	//volatile int *x = (int *)0xe000102c;
	//while ((*x&16)==16);
	 *(int*) 0xe0001030= c;
	//printf("%c\n",(char)c);
}



void hprintf(const char *format, ...){
	int i;
	va_list ap;
	va_start(ap, format);
	for (i = 0; format[i]; i++)
			hprintf_c(format[i]);
	va_end(ap);
}



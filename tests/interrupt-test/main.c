#include <stdint.h>
#include <stdarg.h>
//#include <stdio.h>

//extern void printm();

//#define printf printm

void interrupt_init();

char* hscanf();
void hprintf(const char *format, ...);
#define STACK_POINTER 0x80004000

//__asm__("li sp,0x80004000");
int main_func()
{	

	interrupt_init();

	for (int i=0;i<100;i++){
		hprintf("hello_world  %d\n",i);
	}
	while (1);
}



void hprintf_c(int c){
	// volatile char* serial_base = (char*) OUTPORT;
	// *serial_base = c;

	volatile int *x = (int *)0xe000102c;
	while ((*x&16)==16);
	 *(int*) 0xe0001030= c;
}

void hprintf_s(char *s){
	while(*s) hprintf_c(*(s++));
}

void hprintf_d(int val){
	char buffer[32];
	char *p = buffer;
	if (val < 0) {
		hprintf_c('-');
		val = -val;
	}
	while (val || p == buffer) {
		*(p++) = '0' + val % 10;
		val = val / 10;
	}
	while (p != buffer)
		hprintf_c(*(--p));
}

void hprintf_h(unsigned int val, int digits){
	for (int i = (4*digits)-4; i >= 0; i -= 4)
		hprintf_c("0123456789ABCDEF"[(val >> i) % 16]);
}

void hprintf(const char *format, ...){
	int i;
	va_list ap;
	va_start(ap, format);
	for (i = 0; format[i]; i++)
		if (format[i] == '%') {
			while (format[++i]) {
				if (format[i] == 'c') {
					hprintf_c(va_arg(ap, int));
					break;
				}
				if (format[i] == 's') {
					hprintf_s(va_arg(ap,char*));
					break;
				}
				if (format[i] == 'd') {
					hprintf_d(va_arg(ap,int));
					break;
				}
				//own imp bgn
				if (format[i] == 'u') {
					hprintf_d(va_arg(ap,unsigned int));
					break;
				}
				if (format[i] == 'l') {
					if(format[i+1] == 'u'){
						i++;
						hprintf_d(va_arg(ap,unsigned long));
					}
					else{
						hprintf_d(va_arg(ap,long));
					}
					break;
				}
				if (format[i] == '0') {
					hprintf_h(va_arg(ap,unsigned int),(int)(format[i+1]-'0'));
					i+=2;
					break;
				}
				/*if (format[i] == 'f') {
					float flt=va_arg(ap,double);
					printf_d((int)flt);
					printf_c('.');
					printf_d((int)(((float)flt-(int)flt)*1000));
					break;
				}*/
				//own imp end
			}
		} else
			hprintf_c(format[i]);
	va_end(ap);
}

char hscanf_c(){
  volatile int *x = (int*)0xe000102c;
  if((*x&0x2)==2){                     //checking the data available bit in line status reg
    return -1;
  }else{
    int *c =(int *) 0xe0001030;
    return *c;
  }
}

char* hscanf(){
	char* str="";
	char *r = str;
	volatile char c = hscanf_c();
	while(c != 'n'){
		*(str++) = c;
	}
	return r;
}

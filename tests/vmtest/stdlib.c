// This is free and unencumbered software released into the public domain.
//
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any
// means.

#include "firmware.h"
#include "stdlib.h"
#include "encoding.h"

#include <stdarg.h>
#include <stdint.h>

char heap_memory[1024];
int heap_memory_used = 0;

long time(){
	return read_csr(cycle);
}

long insn(){
	return read_csr(instret);
}

void enableIntr(){
	set_csr(mstatus, MSTATUS_MIE);		// enable interrupt
}

void enableExternalIntr(){
	set_csr(mie, MIP_MEIP);				// enable external interrupt in machine mode
	set_csr(mstatus, MSTATUS_MIE);
}

void enableTimerIntr(){
	set_csr(mie, MIP_MTIP);				// enable timer interrupt in machine mode
	set_csr(mstatus, MSTATUS_MIE);
}

void disableIntr(){
	clear_csr(mstatus, MSTATUS_MIE);	// disable interrupt
}

void disableExternalIntr(){
	clear_csr(mie, MIP_MEIP);			// disable external interrupt in machine mode
}

void disableTimerIntr(){
	clear_csr(mie, MIP_MTIP);			// disable timer interrupt in machine mode
}

unsigned long readMtime(){
	volatile int *x_l = (int *)(MTIME_BASE+4);
	return *x_l;
}

long readMtimeCmp(){
	volatile int *x_l = (int *)(MTIME_CMP_BASE+4);
	return *x_l;
}

void writeMtimeCmp(int cmp){
	volatile int *x_l = (int *)(MTIME_CMP_BASE+4);
	*x_l=cmp;
}

char *malloc(int size){
	char *p = heap_memory + heap_memory_used;
	// printf("[malloc(%d) -> %d (%d..%d)]", size, (int)p, heap_memory_used, heap_memory_used + size);
	heap_memory_used += size;
	if (heap_memory_used > 1024)
		asm("ebreak");
	return p;
}

void printf_c(int c){
	//volatile char* serial_base = (char*) OUTPORT;
	//*serial_base = c;
	volatile int *x = (int *)0xe000102c;
	while ((*x&16)==16);
	*(int*) 0xe0001030= c;
}

void printf_s(char *s){
	while(*s) printf_c(*(s++));
}

void printf_d(int val){
	char buffer[32];
	char *p = buffer;
	if (val < 0) {
		printf_c('-');
		val = -val;
	}
	while (val || p == buffer) {
		*(p++) = '0' + val % 10;
		val = val / 10;
	}
	while (p != buffer)
		printf_c(*(--p));
}

void printf_h(unsigned int val, int digits){
	for (int i = (4*digits)-4; i >= 0; i -= 4)
		printf_c("0123456789ABCDEF"[(val >> i) % 16]);
}

void printf(const char *format, ...){
	int i;
	va_list ap;
	va_start(ap, format);
	for (i = 0; format[i]; i++)
		if (format[i] == '%') {
			while (format[++i]) {
				if (format[i] == 'c') {
					printf_c(va_arg(ap, int));
					break;
				}
				if (format[i] == 's') {
					printf_s(va_arg(ap,char*));
					break;
				}
				if (format[i] == 'd') {
					printf_d(va_arg(ap,int));
					break;
				}
				//own imp bgn
				if (format[i] == 'u') {
					printf_d(va_arg(ap,unsigned int));
					break;
				}
				if (format[i] == 'l') {
					if(format[i+1] == 'u'){
						i++;
						printf_d(va_arg(ap,unsigned long));
					}
					else{
						printf_d(va_arg(ap,long));
					}
					break;
				}
				if (format[i] == '0') {
					printf_h(va_arg(ap,unsigned int),(int)(format[i+1]-'0'));
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
			printf_c(format[i]);
	va_end(ap);
}


char scanf_c(){
	volatile int *x = (int*)0xe000102c;
	while ((*x&2)==2);
	volatile char c =*(int*)0xe0001030;
	printf_c(c);
	return c;
}

char* scanf(){
	char* str="";
	char *r = str;
	volatile char c = scanf_c();
	while(c != 'n'){
		*(str++) = c;
	}
	return r;
}






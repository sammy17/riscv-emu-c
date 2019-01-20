/*
 * init.c
 *
 *  Created on: Nov 4, 2017
 *      Author: ijaz
 */

#include "stdlib.h"

extern void isaTest();
extern void dhrystoneBenchmark();
extern void coremarkBenchmark();

void risvISATestStartMsg(void){
	printf("\n\n------------------------ RISC-V ISA Test's Begins ------------------------\n");
}

void risvDhrystoneStartMsg(void){
	printf("\n\n------------------------ Dhrystone Benchmark Begins ------------------------\n");
}

void risvCoremarkStartMsg(void){
	printf("\n\n------------------------ Coremark Benchmark Begins ------------------------\n");
}


void init(void){
	while(1){
		printf("\n------------------------ Welcome to RISC-V ------------------------\n");
		printf("1) ISA Standard Tests\n");
		printf("2) DHRYSTONE Benchmark\n");
		printf("3) COREMARK  Benchmark\n");
		printf("4) Exit\n");
		printf("Please Select a Choice\t:\t");
		char c = scanf_c();
		if(c=='1'){
			risvISATestStartMsg();
			isaTest();
		}
		else if(c=='2'){
			dhrystoneBenchmark();
		}
		if(c=='3'){
			coremarkBenchmark();
		}
		if(c=='4'){
			while(1);
		}
		else{
			printf("Invalid Input\n");
		}
	}
}

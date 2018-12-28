comp :
	g++ emu.cpp -o emu -std=c++11

run : comp
	./emu

comp_debug :
	g++ emu.cpp -o emu -DDEBUG=1

run_debug : comp_debug
	./emu

clean : 
	rm -rf emu

all : run
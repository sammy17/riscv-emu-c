all : run

comp :
	g++ emu.cpp -o emu -std=c++11

image :
	python comp.py bbl

run : comp image
	./emu

comp_debug :
	g++ emu.cpp -o emu -DDEBUG=1

run_debug : comp_debug
	./emu

clean : 
	rm -rf emu


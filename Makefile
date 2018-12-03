comp :
	g++ emu.cpp -o emu

run : comp
	./emu

clean : 
	rm -rf emu

all : run
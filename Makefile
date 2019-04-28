all : comp

comp :
	g++ emu.cpp -o emu -std=c++11 

comp_with_temu : temu_comp
	g++ -c emu.cpp -o emu.o -std=c++11 -DTEMU
	g++ -o emu  -DTEMU emu.o temu/virtio.o temu/pci.o temu/fs.o temu/cutils.o temu/iomem.o temu/simplefb.o temu/json.o temu/machine.o temu/temu.o temu/slirp/bootp.o temu/slirp/ip_icmp.o temu/slirp/mbuf.o temu/slirp/slirp.o temu/slirp/tcp_output.o temu/slirp/cksum.o temu/slirp/ip_input.o temu/slirp/misc.o temu/slirp/socket.o temu/slirp/tcp_subr.o temu/slirp/udp.o temu/slirp/if.o temu/slirp/ip_output.o temu/slirp/sbuf.o temu/slirp/tcp_input.o temu/slirp/tcp_timer.o temu/fs_disk.o temu/fs_net.o temu/fs_wget.o temu/fs_utils.o temu/block_net.o temu/sdl.o temu/riscv_machine.o temu/softfp.o temu/riscv_cpu32.o temu/riscv_cpu64.o temu/riscv_cpu128.o temu/x86_cpu.o temu/x86_machine.o temu/ide.o temu/ps2.o temu/vmmouse.o temu/pckbd.o temu/vga.o -lrt -lcurl -lcrypto -lSDL

image :
	python comp.py bbl

run : comp 
	./emu

comp_debug :
	g++ emu.cpp -o emu -DDEBUG=1

run_debug : comp_debug
	./emu

clean : 
	rm -rf emu.o temu/*.o temu/slirp/*.o data_hex.txt g.hex test.txt

temu_comp:
	cd temu && gcc -O2 -Wall -g -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -MMD -D_GNU_SOURCE -DCONFIG_VERSION=\"2019-02-10\" -DCONFIG_SLIRP -DCONFIG_FS_NET -DCONFIG_SDL -DCONFIG_RISCV_MAX_XLEN=128 -DCONFIG_X86EMU -c -o virtio.o virtio.c
	cd temu && gcc -O2 -Wall -g -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -MMD -D_GNU_SOURCE -DCONFIG_VERSION=\"2019-02-10\" -DCONFIG_SLIRP -DCONFIG_FS_NET -DCONFIG_SDL -DCONFIG_RISCV_MAX_XLEN=128 -DCONFIG_X86EMU -c -o pci.o pci.c
	cd temu && gcc -O2 -Wall -g -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -MMD -D_GNU_SOURCE -DCONFIG_VERSION=\"2019-02-10\" -DCONFIG_SLIRP -DCONFIG_FS_NET -DCONFIG_SDL -DCONFIG_RISCV_MAX_XLEN=128 -DCONFIG_X86EMU -c -o fs.o fs.c
	cd temu && gcc -O2 -Wall -g -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -MMD -D_GNU_SOURCE -DCONFIG_VERSION=\"2019-02-10\" -DCONFIG_SLIRP -DCONFIG_FS_NET -DCONFIG_SDL -DCONFIG_RISCV_MAX_XLEN=128 -DCONFIG_X86EMU -c -o cutils.o cutils.c
	cd temu && gcc -O2 -Wall -g -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -MMD -D_GNU_SOURCE -DCONFIG_VERSION=\"2019-02-10\" -DCONFIG_SLIRP -DCONFIG_FS_NET -DCONFIG_SDL -DCONFIG_RISCV_MAX_XLEN=128 -DCONFIG_X86EMU -c -o iomem.o iomem.c
	cd temu && gcc -O2 -Wall -g -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -MMD -D_GNU_SOURCE -DCONFIG_VERSION=\"2019-02-10\" -DCONFIG_SLIRP -DCONFIG_FS_NET -DCONFIG_SDL -DCONFIG_RISCV_MAX_XLEN=128 -DCONFIG_X86EMU -c -o simplefb.o simplefb.c
	cd temu && gcc -O2 -Wall -g -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -MMD -D_GNU_SOURCE -DCONFIG_VERSION=\"2019-02-10\" -DCONFIG_SLIRP -DCONFIG_FS_NET -DCONFIG_SDL -DCONFIG_RISCV_MAX_XLEN=128 -DCONFIG_X86EMU -c -o json.o json.c
	cd temu && gcc -O2 -Wall -g -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -MMD -D_GNU_SOURCE -DCONFIG_VERSION=\"2019-02-10\" -DCONFIG_SLIRP -DCONFIG_FS_NET -DCONFIG_SDL -DCONFIG_RISCV_MAX_XLEN=128 -DCONFIG_X86EMU -c -o machine.o machine.c
	cd temu && gcc -O2 -Wall -g -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -MMD -D_GNU_SOURCE -DCONFIG_VERSION=\"2019-02-10\" -DCONFIG_SLIRP -DCONFIG_FS_NET -DCONFIG_SDL -DCONFIG_RISCV_MAX_XLEN=128 -DCONFIG_X86EMU -c -o temu.o temu.c
	cd temu && gcc -O2 -Wall -g -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -MMD -D_GNU_SOURCE -DCONFIG_VERSION=\"2019-02-10\" -DCONFIG_SLIRP -DCONFIG_FS_NET -DCONFIG_SDL -DCONFIG_RISCV_MAX_XLEN=128 -DCONFIG_X86EMU -c -o slirp/bootp.o slirp/bootp.c
	cd temu && gcc -O2 -Wall -g -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -MMD -D_GNU_SOURCE -DCONFIG_VERSION=\"2019-02-10\" -DCONFIG_SLIRP -DCONFIG_FS_NET -DCONFIG_SDL -DCONFIG_RISCV_MAX_XLEN=128 -DCONFIG_X86EMU -c -o slirp/ip_icmp.o slirp/ip_icmp.c
	cd temu && gcc -O2 -Wall -g -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -MMD -D_GNU_SOURCE -DCONFIG_VERSION=\"2019-02-10\" -DCONFIG_SLIRP -DCONFIG_FS_NET -DCONFIG_SDL -DCONFIG_RISCV_MAX_XLEN=128 -DCONFIG_X86EMU -c -o slirp/mbuf.o slirp/mbuf.c
	cd temu && gcc -O2 -Wall -g -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -MMD -D_GNU_SOURCE -DCONFIG_VERSION=\"2019-02-10\" -DCONFIG_SLIRP -DCONFIG_FS_NET -DCONFIG_SDL -DCONFIG_RISCV_MAX_XLEN=128 -DCONFIG_X86EMU -c -o slirp/slirp.o slirp/slirp.c
	cd temu && gcc -O2 -Wall -g -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -MMD -D_GNU_SOURCE -DCONFIG_VERSION=\"2019-02-10\" -DCONFIG_SLIRP -DCONFIG_FS_NET -DCONFIG_SDL -DCONFIG_RISCV_MAX_XLEN=128 -DCONFIG_X86EMU -c -o slirp/tcp_output.o slirp/tcp_output.c
	cd temu && gcc -O2 -Wall -g -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -MMD -D_GNU_SOURCE -DCONFIG_VERSION=\"2019-02-10\" -DCONFIG_SLIRP -DCONFIG_FS_NET -DCONFIG_SDL -DCONFIG_RISCV_MAX_XLEN=128 -DCONFIG_X86EMU -c -o slirp/cksum.o slirp/cksum.c
	cd temu && gcc -O2 -Wall -g -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -MMD -D_GNU_SOURCE -DCONFIG_VERSION=\"2019-02-10\" -DCONFIG_SLIRP -DCONFIG_FS_NET -DCONFIG_SDL -DCONFIG_RISCV_MAX_XLEN=128 -DCONFIG_X86EMU -c -o slirp/ip_input.o slirp/ip_input.c
	cd temu && gcc -O2 -Wall -g -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -MMD -D_GNU_SOURCE -DCONFIG_VERSION=\"2019-02-10\" -DCONFIG_SLIRP -DCONFIG_FS_NET -DCONFIG_SDL -DCONFIG_RISCV_MAX_XLEN=128 -DCONFIG_X86EMU -c -o slirp/misc.o slirp/misc.c
	cd temu && gcc -O2 -Wall -g -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -MMD -D_GNU_SOURCE -DCONFIG_VERSION=\"2019-02-10\" -DCONFIG_SLIRP -DCONFIG_FS_NET -DCONFIG_SDL -DCONFIG_RISCV_MAX_XLEN=128 -DCONFIG_X86EMU -c -o slirp/socket.o slirp/socket.c
	cd temu && gcc -O2 -Wall -g -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -MMD -D_GNU_SOURCE -DCONFIG_VERSION=\"2019-02-10\" -DCONFIG_SLIRP -DCONFIG_FS_NET -DCONFIG_SDL -DCONFIG_RISCV_MAX_XLEN=128 -DCONFIG_X86EMU -c -o slirp/tcp_subr.o slirp/tcp_subr.c
	cd temu && gcc -O2 -Wall -g -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -MMD -D_GNU_SOURCE -DCONFIG_VERSION=\"2019-02-10\" -DCONFIG_SLIRP -DCONFIG_FS_NET -DCONFIG_SDL -DCONFIG_RISCV_MAX_XLEN=128 -DCONFIG_X86EMU -c -o slirp/udp.o slirp/udp.c
	cd temu && gcc -O2 -Wall -g -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -MMD -D_GNU_SOURCE -DCONFIG_VERSION=\"2019-02-10\" -DCONFIG_SLIRP -DCONFIG_FS_NET -DCONFIG_SDL -DCONFIG_RISCV_MAX_XLEN=128 -DCONFIG_X86EMU -c -o slirp/if.o slirp/if.c
	cd temu && gcc -O2 -Wall -g -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -MMD -D_GNU_SOURCE -DCONFIG_VERSION=\"2019-02-10\" -DCONFIG_SLIRP -DCONFIG_FS_NET -DCONFIG_SDL -DCONFIG_RISCV_MAX_XLEN=128 -DCONFIG_X86EMU -c -o slirp/ip_output.o slirp/ip_output.c
	cd temu && gcc -O2 -Wall -g -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -MMD -D_GNU_SOURCE -DCONFIG_VERSION=\"2019-02-10\" -DCONFIG_SLIRP -DCONFIG_FS_NET -DCONFIG_SDL -DCONFIG_RISCV_MAX_XLEN=128 -DCONFIG_X86EMU -c -o slirp/sbuf.o slirp/sbuf.c
	cd temu && gcc -O2 -Wall -g -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -MMD -D_GNU_SOURCE -DCONFIG_VERSION=\"2019-02-10\" -DCONFIG_SLIRP -DCONFIG_FS_NET -DCONFIG_SDL -DCONFIG_RISCV_MAX_XLEN=128 -DCONFIG_X86EMU -c -o slirp/tcp_input.o slirp/tcp_input.c
	cd temu && gcc -O2 -Wall -g -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -MMD -D_GNU_SOURCE -DCONFIG_VERSION=\"2019-02-10\" -DCONFIG_SLIRP -DCONFIG_FS_NET -DCONFIG_SDL -DCONFIG_RISCV_MAX_XLEN=128 -DCONFIG_X86EMU -c -o slirp/tcp_timer.o slirp/tcp_timer.c
	cd temu && gcc -O2 -Wall -g -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -MMD -D_GNU_SOURCE -DCONFIG_VERSION=\"2019-02-10\" -DCONFIG_SLIRP -DCONFIG_FS_NET -DCONFIG_SDL -DCONFIG_RISCV_MAX_XLEN=128 -DCONFIG_X86EMU -c -o fs_disk.o fs_disk.c
	cd temu && gcc -O2 -Wall -g -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -MMD -D_GNU_SOURCE -DCONFIG_VERSION=\"2019-02-10\" -DCONFIG_SLIRP -DCONFIG_FS_NET -DCONFIG_SDL -DCONFIG_RISCV_MAX_XLEN=128 -DCONFIG_X86EMU -c -o fs_net.o fs_net.c
	cd temu && gcc -O2 -Wall -g -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -MMD -D_GNU_SOURCE -DCONFIG_VERSION=\"2019-02-10\" -DCONFIG_SLIRP -DCONFIG_FS_NET -DCONFIG_SDL -DCONFIG_RISCV_MAX_XLEN=128 -DCONFIG_X86EMU -c -o fs_wget.o fs_wget.c
	cd temu && gcc -O2 -Wall -g -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -MMD -D_GNU_SOURCE -DCONFIG_VERSION=\"2019-02-10\" -DCONFIG_SLIRP -DCONFIG_FS_NET -DCONFIG_SDL -DCONFIG_RISCV_MAX_XLEN=128 -DCONFIG_X86EMU -c -o fs_utils.o fs_utils.c
	cd temu && gcc -O2 -Wall -g -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -MMD -D_GNU_SOURCE -DCONFIG_VERSION=\"2019-02-10\" -DCONFIG_SLIRP -DCONFIG_FS_NET -DCONFIG_SDL -DCONFIG_RISCV_MAX_XLEN=128 -DCONFIG_X86EMU -c -o block_net.o block_net.c
	cd temu && gcc -O2 -Wall -g -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -MMD -D_GNU_SOURCE -DCONFIG_VERSION=\"2019-02-10\" -DCONFIG_SLIRP -DCONFIG_FS_NET -DCONFIG_SDL -DCONFIG_RISCV_MAX_XLEN=128 -DCONFIG_X86EMU -c -o sdl.o sdl.c
	cd temu && gcc -O2 -Wall -g -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -MMD -D_GNU_SOURCE -DCONFIG_VERSION=\"2019-02-10\" -DCONFIG_SLIRP -DCONFIG_FS_NET -DCONFIG_SDL -DCONFIG_RISCV_MAX_XLEN=128 -DCONFIG_X86EMU -c -o riscv_machine.o riscv_machine.c
	cd temu && gcc -O2 -Wall -g -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -MMD -D_GNU_SOURCE -DCONFIG_VERSION=\"2019-02-10\" -DCONFIG_SLIRP -DCONFIG_FS_NET -DCONFIG_SDL -DCONFIG_RISCV_MAX_XLEN=128 -DCONFIG_X86EMU -c -o softfp.o softfp.c
	cd temu && gcc -O2 -Wall -g -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -MMD -D_GNU_SOURCE -DCONFIG_VERSION=\"2019-02-10\" -DCONFIG_SLIRP -DCONFIG_FS_NET -DCONFIG_SDL -DCONFIG_RISCV_MAX_XLEN=128 -DCONFIG_X86EMU -DMAX_XLEN=32 -c -o riscv_cpu32.o riscv_cpu.c
	cd temu && gcc -O2 -Wall -g -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -MMD -D_GNU_SOURCE -DCONFIG_VERSION=\"2019-02-10\" -DCONFIG_SLIRP -DCONFIG_FS_NET -DCONFIG_SDL -DCONFIG_RISCV_MAX_XLEN=128 -DCONFIG_X86EMU -DMAX_XLEN=64 -c -o riscv_cpu64.o riscv_cpu.c
	cd temu && gcc -O2 -Wall -g -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -MMD -D_GNU_SOURCE -DCONFIG_VERSION=\"2019-02-10\" -DCONFIG_SLIRP -DCONFIG_FS_NET -DCONFIG_SDL -DCONFIG_RISCV_MAX_XLEN=128 -DCONFIG_X86EMU -DMAX_XLEN=128 -c -o riscv_cpu128.o riscv_cpu.c
	cd temu && gcc -O2 -Wall -g -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -MMD -D_GNU_SOURCE -DCONFIG_VERSION=\"2019-02-10\" -DCONFIG_SLIRP -DCONFIG_FS_NET -DCONFIG_SDL -DCONFIG_RISCV_MAX_XLEN=128 -DCONFIG_X86EMU -c -o x86_cpu.o x86_cpu.c
	cd temu && gcc -O2 -Wall -g -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -MMD -D_GNU_SOURCE -DCONFIG_VERSION=\"2019-02-10\" -DCONFIG_SLIRP -DCONFIG_FS_NET -DCONFIG_SDL -DCONFIG_RISCV_MAX_XLEN=128 -DCONFIG_X86EMU -c -o x86_machine.o x86_machine.c
	cd temu && gcc -O2 -Wall -g -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -MMD -D_GNU_SOURCE -DCONFIG_VERSION=\"2019-02-10\" -DCONFIG_SLIRP -DCONFIG_FS_NET -DCONFIG_SDL -DCONFIG_RISCV_MAX_XLEN=128 -DCONFIG_X86EMU -c -o ide.o ide.c
	cd temu && gcc -O2 -Wall -g -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -MMD -D_GNU_SOURCE -DCONFIG_VERSION=\"2019-02-10\" -DCONFIG_SLIRP -DCONFIG_FS_NET -DCONFIG_SDL -DCONFIG_RISCV_MAX_XLEN=128 -DCONFIG_X86EMU -c -o ps2.o ps2.c
	cd temu && gcc -O2 -Wall -g -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -MMD -D_GNU_SOURCE -DCONFIG_VERSION=\"2019-02-10\" -DCONFIG_SLIRP -DCONFIG_FS_NET -DCONFIG_SDL -DCONFIG_RISCV_MAX_XLEN=128 -DCONFIG_X86EMU -c -o vmmouse.o vmmouse.c
	cd temu && gcc -O2 -Wall -g -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -MMD -D_GNU_SOURCE -DCONFIG_VERSION=\"2019-02-10\" -DCONFIG_SLIRP -DCONFIG_FS_NET -DCONFIG_SDL -DCONFIG_RISCV_MAX_XLEN=128 -DCONFIG_X86EMU -c -o pckbd.o pckbd.c
	cd temu && gcc -O2 -Wall -g -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -MMD -D_GNU_SOURCE -DCONFIG_VERSION=\"2019-02-10\" -DCONFIG_SLIRP -DCONFIG_FS_NET -DCONFIG_SDL -DCONFIG_RISCV_MAX_XLEN=128 -DCONFIG_X86EMU -c -o vga.o vga.c   


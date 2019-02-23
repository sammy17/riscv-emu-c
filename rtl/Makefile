# This makefile handles running vcs simulation, compiling isa/benchmark tests, and running python emulator on the compiled c codes
vcs: 
	vcs -q -full64 -f flist.f -sverilog +incdir+./src/design/common +incdir+./src/design/newcache  -j20 -debug_access+all +vcs+fsdbon+mda -kdb -lca -l vcs.log; ./simv -l run.log
nodebug:	
	vcs -full64 -f flist.f -sverilog +incdir+./src/design/common +incdir+./src/design/newcache -fgp -j20; ./simv  

clean:
	rm -rf  csrc inter.vpd simv* simv.daidir ucli.key vc_hdrs.h vcs.log   novas* run.log *.vpd nWaveLog BSS* urg* verdiLog comp.log *.mem DVEfiles fsdbextractLog WRITEMEM AN.DB/ work*

riscv_test_compile:
	cd Test_Benchmark/Debug/ && $(MAKE) clean && $(MAKE) all
	

copy_elf_files:
	cp Test_Benchmark/Debug/RISCV_Test_Benchmark.* Soft_Processor/

run_emulator:
	cd Soft_Processor/ && python conv.py && python emu.py


all: riscv_test_compile copy_elf_files run_emulator
	

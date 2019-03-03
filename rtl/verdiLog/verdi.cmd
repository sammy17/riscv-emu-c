simSetSimulator "-vcssv" -exec \
           "/remote/vgrnd21/subashar/risc64/riscv-emu-c/rtl/simv" -args \
           "-fgp:numcores=10"
debImport "-dbdir" "/remote/vgrnd21/subashar/risc64/riscv-emu-c/rtl/simv.daidir"
debLoadSimResult /remote/vgrnd21/subashar/risc64/riscv-emu-c/rtl/novas.fsdb
wvCreateWindow
srcHBSelect "Test_RISCV_PROCESSOR.uut" -win $_nTrace1
srcSetScope -win $_nTrace1 "Test_RISCV_PROCESSOR.uut" -delim "."
srcHBSelect "Test_RISCV_PROCESSOR.uut" -win $_nTrace1
srcHBSelect "Test_RISCV_PROCESSOR.uut.itlb" -win $_nTrace1
srcSetScope -win $_nTrace1 "Test_RISCV_PROCESSOR.uut.itlb" -delim "."
srcHBSelect "Test_RISCV_PROCESSOR.uut.itlb" -win $_nTrace1
srcDeselectAll -win $_nTrace1
srcSelect -signal "PHY_ADDR" -line 28 -pos 1 -win $_nTrace1
srcDeselectAll -win $_nTrace1
srcSelect -signal "PHY_ADDR_VALID" -line 27 -pos 1 -win $_nTrace1
srcAddSelectedToWave -clipboard -win $_nTrace1
wvDrop -win $_nWave2
wvZoomOut -win $_nWave2
wvZoomOut -win $_nWave2
wvZoomOut -win $_nWave2
wvZoomOut -win $_nWave2
wvZoomOut -win $_nWave2
wvZoom -win $_nWave2 461169499.063670 6594723836.610487
wvZoom -win $_nWave2 1322623759.955007 2327653730.994981
wvZoom -win $_nWave2 1470366929.855071 1701862597.341807
wvZoom -win $_nWave2 1533876509.043777 1624914131.089122
wvZoomOut -win $_nWave2
wvZoomOut -win $_nWave2
wvZoomOut -win $_nWave2
wvZoomOut -win $_nWave2
wvZoomOut -win $_nWave2
wvZoomOut -win $_nWave2
wvZoomOut -win $_nWave2
wvZoomOut -win $_nWave2
wvZoomOut -win $_nWave2
wvZoom -win $_nWave2 46116949.906367 5557092463.717229
wvZoom -win $_nWave2 46116949.906360 169959096.284132
wvZoom -win $_nWave2 46116949.906360 53306287.991961
wvZoom -win $_nWave2 48439348.445248 53023561.213314
wvZoomOut -win $_nWave2
wvZoomOut -win $_nWave2
wvZoomOut -win $_nWave2
wvZoomOut -win $_nWave2
wvZoomOut -win $_nWave2
wvZoomOut -win $_nWave2
wvZoomOut -win $_nWave2
wvZoomOut -win $_nWave2
wvZoomOut -win $_nWave2
wvZoomOut -win $_nWave2
wvZoomOut -win $_nWave2
wvZoomOut -win $_nWave2
wvZoomOut -win $_nWave2
debExit

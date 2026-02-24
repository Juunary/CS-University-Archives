vvp tb_PC.o -fst
FC /L mem_dump_SS.txt mem_dump.txt
FC /L reg_dump_SS.txt reg_dump.txt
pause
gtkwave tb_PC.vcd

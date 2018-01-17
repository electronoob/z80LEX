
program:
	gcc ./parse.c && ./a.exe  --input z80.asm
check:
	cppcheck --enable=all --inconclusive ./parse.c

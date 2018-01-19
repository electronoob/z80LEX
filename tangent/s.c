#include <stdio.h>
#include <string.h>

char string[] = "; 16-bit Integer to ASCII (decimal)\n; http://map.grauw.nl/sources/external/z80bits.html#5.1\n;\n; Input: HL = number to convert, DE = location of ASCII string\n; Output: ASCII string at (DE)\n\n\nNum2Dec:\n\tld\tbc,-10000\n\tcall\tNum1\n\tld\tbc,-1000\n\tcall\tNum1\n\tld\tbc,-100\n\tcall\tNum1\n\tld\tc,-10\n\tcall\tNum1\n\tld\tc,b\n\nNum1:\t\n\tld\ta,'0'-1\nNum2:\t\n\tinc\ta\n\tadd\thl,bc\n\tjr\tc,Num2\n\tsbc\thl,bc\n\n\tld\t[de],a\n\tinc\tde\n\tret\n";
int string_len;

struct parser_struct
{
	int c_line;
	int c_char;
	int c_offset;
	int (*next)();
	int (*prev)();
	int (*bookmark)();
};
typedef struct parser_struct parser;

parser p = {};

int next(parser this)
{
	printf("next %i\n", this.c_offset);
	return -1;
}

int prev(parser this)
{
	printf("prev\n");
	return 1;
}

int init()
{
	p.next = &next;
	p.prev = &prev;
	return 0;
}

int main()
{
	init();
	string_len = strlen(string);
	p.next(p);
	return 0;
}



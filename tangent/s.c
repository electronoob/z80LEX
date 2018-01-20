#include <stdio.h>
#include <string.h>
char string[] = "; 16-bit Integer to ASCII (decimal)\n; http://map.grauw.nl/sources/external/z80bits.html#5.1\n;\n; Input: HL = number to convert, DE = location of ASCII string\n; Output: ASCII string at (DE)\n\n\nNum2Dec:\n\tld\tbc,-10000\n\tcall\tNum1\n\tld\tbc,-1000\n\tcall\tNum1\n\tld\tbc,-100\n\tcall\tNum1\n\tld\tc,-10\n\tcall\tNum1\n\tld\tc,b\n\nNum1:\t\n\tld\ta,'0'-1\nNum2:\t\n\tinc\ta\n\tadd\thl,bc\n\tjr\tc,Num2\n\tsbc\thl,bc\n\n\tld\t[de],a\n\tinc\tde\n\tret\n";
int string_len;
struct parser_struct
{
	int x;
	int y;
	int o;
	int (*next)();
	int (*prev)();
	int (*bookmark)();
};
typedef struct parser_struct parser;

parser p = {};

int next(parser this)
{
	char whitespace[] = " \t\n";
	int ws_len = 3,i,k;
	uint8_t a, b;

	for(i=p.o; i<string_len;i++)
	{
		a = string[i];
		for(k=0; k<ws_len;k++)
		{
			b = whitespace[k];
			if(a == b)
			{
				p.o = i+1;
				return 1;
			}
		}
	}
	return 0;
}

int prev(parser this)
{
	printf("prev\n");
	return 0;
}
int bookmark(parser this)
{
	printf("bookmark\n");
	return 0;
}
int init()
{
	p.next = &next;
	p.prev = &prev;
	p.bookmark = &bookmark;
	return 0;
}

int main()
{
	init();
	string_len = strlen(string);
	printf("[%c]", string[p.o]);
	p.next(&p);
	printf("[%c]", string[p.o]);
	p.next(&p);
	printf("[%c]", string[p.o]);
	p.next(&p);
	printf("[%c]", string[p.o]);
	p.next(&p);
	printf("[%c]", string[p.o]);
	p.next(&p);
	printf("[%c]", string[p.o]);
	p.next(&p);
	printf("[%c]", string[p.o]);
	p.next(&p);
	printf("[%c]", string[p.o]);
	p.next(&p);
	printf("[%c]", string[p.o]);
	p.next(&p);
	printf("[%c]", string[p.o]);
	p.next(&p);
	printf("[%c]", string[p.o]);
	p.next(&p);
	printf("[%c]", string[p.o]);
	p.next(&p);
	printf("[%c]", string[p.o]);
	p.next(&p);
	printf("[%c]", string[p.o]);
	p.next(&p);
	printf("[%c]", string[p.o]);
	p.next(&p);
	printf("[%c]", string[p.o]);
	p.next(&p);
	return 0;
}



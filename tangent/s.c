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
char whitespace[] = "; \t\n";
int ws_len = 4;

int next(parser this)
{
	uint8_t a, b, c;
	int i,k,m;
	for(i=this.o; i<string_len;i++)
	{
		a = string[i];
		for(k=0; k<ws_len;k++)
		{
			b = whitespace[k];
			if(a == b)
			{
				if(a == ';')
				{
					for(m=i; m<string_len;m++)
					{
						c = string[m];
						if (c == '\n')
						{
							this.o = m+1;
							return 1;
						}
					}
				} else {
					this.o = i+1;
					return 1;
				}
			}
		}
	}
	this.o = string_len;
	return 0;
}

int prev(parser this)
{
	uint8_t a, b;
	int i,k;	
	for(i=this.o; i >= 0; i--)
	{
		a = string[i];
		for(k=0; k<ws_len;k++)
		{
			b = whitespace[k];
			if(a == b)
			{
				this.o = i-1;
				return 1;
			}
		}
	}
	this.o = 0;
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
#define NEXT p.next(&p);
#define PREV p.prev(&p);p.prev(&p);p.next(&p);
#define PRINT printf("[%c]", string[p.o]);
int main()
{
	init();
	string_len = strlen(string);	

	PRINT;NEXT;
	PRINT;NEXT;
	PRINT;NEXT;
	PRINT;NEXT;
	PRINT;NEXT;
		PRINT;NEXT;
	PRINT;NEXT;
	PRINT;NEXT;
	PRINT;NEXT;
	PRINT;NEXT;
PRINT;NEXT;
	PRINT;NEXT;
	PRINT;NEXT;
	PRINT;NEXT;
	PRINT;NEXT;
		PRINT;NEXT;
	PRINT;NEXT;
	PRINT;NEXT;
	PRINT;NEXT;
	PRINT;NEXT;
	PRINT;


	printf("\n");
			
	return 0;
}



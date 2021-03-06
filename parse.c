#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define TMPF_BUFFER_SIZE 1024

struct cpu_registers
{
	uint16_t SP;
	uint16_t PC;
	struct {
		union {
			struct {
				int8_t A;
				int8_t F;
			};
			struct {
				int16_t AF;
			};
		};
	};
	struct {
		union {
			struct {
				int8_t B;
				int8_t C;
			};
			struct {
				int16_t BC;
			};
		};
	};
	struct {
		union {
			struct {
				int8_t D;
				int8_t E;
			};
			struct {
				int16_t DE;
			};
		};
	};
	struct {
		union {
			struct {
				int8_t H;
				int8_t L;
			};
			struct {
				int16_t HL;
			};
		};
	};
};
typedef struct cpu_registers registers;

registers cpureg = {};

struct nodeStruct
{
  char mnemonic[32];
  char flagZ[2];
  char flagN[2];
  char flagH[2];
  char flagC[2];
  uint8_t len;
  uint8_t cyclesTaken;
  uint8_t cyclesNotTaken;
  uint8_t hex;
};

typedef struct nodeStruct node;

char *process(char *line);
char *label_tbl[64];
uint8_t label_tbl_n = 0;

char *src_filename_buffer;
unsigned char *rawData;
unsigned int rawData_len;

unsigned char * import_file(char * file)
{
  FILE * input;
  input = fopen(file, "rb");
  if (input == NULL)
  {
    fprintf(stderr, "Unable to open file for reading.\n");
    exit(1);
  }
  unsigned int bytes;
  unsigned int the_file_len = 0;
  unsigned char * the_file_data;
  unsigned char * tmp_buffer;
  unsigned char * tmp_ptr;
  tmp_ptr = NULL;
  tmp_buffer = malloc(TMPF_BUFFER_SIZE);
  the_file_data = malloc(TMPF_BUFFER_SIZE);
  memset(the_file_data, 0, TMPF_BUFFER_SIZE);
  while (1)
  {
    memset(tmp_buffer, 0, TMPF_BUFFER_SIZE);
    bytes = fread(tmp_buffer, 1, TMPF_BUFFER_SIZE, input);
    if (bytes == 0)
    {
      break;
    }
    if ((tmp_ptr = realloc(the_file_data, the_file_len + bytes)) == NULL)
    {
      fprintf(stderr, "Unable to realloc().\n");
      exit(1);
    } else {
      the_file_data = tmp_ptr;
      memcpy(the_file_data + the_file_len, tmp_buffer, bytes);
    }
    the_file_len += bytes;
    if (bytes != TMPF_BUFFER_SIZE)
    {
      break;
    }
  }

  rawData_len = the_file_len;
  free(tmp_buffer);
  fclose(input);
  return the_file_data;
}

	void showhelp() {
		printf("example:\n");
		printf("\t./a.out --input z80.asm\n");
	}
	void showabout() {
                //printf("beverly-crusher (build %d) [http://electronoob.com]\n\n",BUILDNUMBER);
                printf("Released under the terms of the MIT license.\nCopyright (c) 2018 electronoob.\nAll rights reserved.\n\n");
	}

int main (int argc, char *argv[])
{

	/* begin processing commandline */
	int i;
	int input_from_file = 0;
	for (i=1;i<argc;i++)
	{	
		if (strcmp("--input", argv[i]) == 0)
		{
        i++;
                                if(argc == i)
				{
					showabout();
					showhelp();
                                        fprintf (stderr, "--input [filename] error. missing filename parameter.\n");
                                        exit(1);
                                }
                                src_filename_buffer = malloc(strlen(argv[i]) +1);
                                if (src_filename_buffer == NULL) 
				{
					showabout();
                                        fprintf (stderr, "--input [filename] error. unable to malloc() memory.\n");
                                        exit(1);
                                }
                                memset(src_filename_buffer, 0, strlen(argv[i]) +1);
                                strcpy(src_filename_buffer, argv[i]);
				input_from_file = 1;
                                continue;
                        }
	}
	if(!input_from_file)
	{
			showabout();
			showhelp();
			fprintf (stderr, "error: --input not set.\n");
			exit(1);
	}
	rawData = import_file(src_filename_buffer);



/* Legend
 *
 *	when opcode is undefined then labeled as "invalid"
 *	when flag is undefined then marked as "u"
 */


node opcode[] = {
  {"NOP","-","-","-","-",1,4,4,0x0},
  {"LD BC,d16","-","-","-","-",3,12,12,0x1},
  {"LD (BC),A","-","-","-","-",1,8,8,0x2},
  {"INC BC","-","-","-","-",1,8,8,0x3},
  {"INC B","Z","0","H","-",1,4,4,0x4},
  {"DEC B","Z","1","H","-",1,4,4,0x5},
  {"LD B,d8","-","-","-","-",2,8,8,0x6},
  {"RLCA","0","0","0","C",1,4,4,0x7},
  {"LD (a16),SP","-","-","-","-",3,20,20,0x8},
  {"ADD HL,BC","-","0","H","C",1,8,8,0x9},
  {"LD A,(BC)","-","-","-","-",1,8,8,0xA},
  {"DEC BC","-","-","-","-",1,8,8,0xB},
  {"INC C","Z","0","H","-",1,4,4,0xC},
  {"DEC C","Z","1","H","-",1,4,4,0xD},
  {"LD C,d8","-","-","-","-",2,8,8,0xE},
  {"RRCA","0","0","0","C",1,4,4,0xF},
  {"STOP 0","-","-","-","-",2,4,4,0x10},
  {"LD DE,d16","-","-","-","-",3,12,12,0x11},
  {"LD (DE),A","-","-","-","-",1,8,8,0x12},
  {"INC DE","-","-","-","-",1,8,8,0x13},
  {"INC D","Z","0","H","-",1,4,4,0x14},
  {"DEC D","Z","1","H","-",1,4,4,0x15},
  {"LD D,d8","-","-","-","-",2,8,8,0x16},
  {"RLA","0","0","0","C",1,4,4,0x17},
  {"JR r8","-","-","-","-",2,12,12,0x18},
  {"ADD HL,DE","-","0","H","C",1,8,8,0x19},
  {"LD A,(DE)","-","-","-","-",1,8,8,0x1A},
  {"DEC DE","-","-","-","-",1,8,8,0x1B},
  {"INC E","Z","0","H","-",1,4,4,0x1C},
  {"DEC E","Z","1","H","-",1,4,4,0x1D},
  {"LD E,d8","-","-","-","-",2,8,8,0x1E},
  {"RRA","0","0","0","C",1,4,4,0x1F},
  {"JR NZ,r8","-","-","-","-",2,12,8,0x20},
  {"LD HL,d16","-","-","-","-",3,12,12,0x21},
  {"LD (HL+),A","-","-","-","-",1,8,8,0x22},
  {"INC HL","-","-","-","-",1,8,8,0x23},
  {"INC H","Z","0","H","-",1,4,4,0x24},
  {"DEC H","Z","1","H","-",1,4,4,0x25},
  {"LD H,d8","-","-","-","-",2,8,8,0x26},
  {"DAA","Z","-","0","C",1,4,4,0x27},
  {"JR Z,r8","-","-","-","-",2,12,8,0x28},
  {"ADD HL,HL","-","0","H","C",1,8,8,0x29},
  {"LD A,(HL+)","-","-","-","-",1,8,8,0x2A},
  {"DEC HL","-","-","-","-",1,8,8,0x2B},
  {"INC L","Z","0","H","-",1,4,4,0x2C},
  {"DEC L","Z","1","H","-",1,4,4,0x2D},
  {"LD L,d8","-","-","-","-",2,8,8,0x2E},
  {"CPL","-","1","1","-",1,4,4,0x2F},
  {"JR NC,r8","-","-","-","-",2,12,8,0x30},
  {"LD SP,d16","-","-","-","-",3,12,12,0x31},
  {"LD (HL-),A","-","-","-","-",1,8,8,0x32},
  {"INC SP","-","-","-","-",1,8,8,0x33},
  {"INC (HL)","Z","0","H","-",1,12,12,0x34},
  {"DEC (HL)","Z","1","H","-",1,12,12,0x35},
  {"LD (HL),d8","-","-","-","-",2,12,12,0x36},
  {"SCF","-","0","0","1",1,4,4,0x37},
  {"JR C,r8","-","-","-","-",2,12,8,0x38},
  {"ADD HL,SP","-","0","H","C",1,8,8,0x39},
  {"LD A,(HL-)","-","-","-","-",1,8,8,0x3A},
  {"DEC SP","-","-","-","-",1,8,8,0x3B},
  {"INC A","Z","0","H","-",1,4,4,0x3C},
  {"DEC A","Z","1","H","-",1,4,4,0x3D},
  {"LD A,d8","-","-","-","-",2,8,8,0x3E},
  {"CCF","-","0","0","C",1,4,4,0x3F},
  {"LD B,B","-","-","-","-",1,4,4,0x40},
  {"LD B,C","-","-","-","-",1,4,4,0x41},
  {"LD B,D","-","-","-","-",1,4,4,0x42},
  {"LD B,E","-","-","-","-",1,4,4,0x43},
  {"LD B,H","-","-","-","-",1,4,4,0x44},
  {"LD B,L","-","-","-","-",1,4,4,0x45},
  {"LD B,(HL)","-","-","-","-",1,8,8,0x46},
  {"LD B,A","-","-","-","-",1,4,4,0x47},
  {"LD C,B","-","-","-","-",1,4,4,0x48},
  {"LD C,C","-","-","-","-",1,4,4,0x49},
  {"LD C,D","-","-","-","-",1,4,4,0x4A},
  {"LD C,E","-","-","-","-",1,4,4,0x4B},
  {"LD C,H","-","-","-","-",1,4,4,0x4C},
  {"LD C,L","-","-","-","-",1,4,4,0x4D},
  {"LD C,(HL)","-","-","-","-",1,8,8,0x4E},
  {"LD C,A","-","-","-","-",1,4,4,0x4F},
  {"LD D,B","-","-","-","-",1,4,4,0x50},
  {"LD D,C","-","-","-","-",1,4,4,0x51},
  {"LD D,D","-","-","-","-",1,4,4,0x52},
  {"LD D,E","-","-","-","-",1,4,4,0x53},
  {"LD D,H","-","-","-","-",1,4,4,0x54},
  {"LD D,L","-","-","-","-",1,4,4,0x55},
  {"LD D,(HL)","-","-","-","-",1,8,8,0x56},
  {"LD D,A","-","-","-","-",1,4,4,0x57},
  {"LD E,B","-","-","-","-",1,4,4,0x58},
  {"LD E,C","-","-","-","-",1,4,4,0x59},
  {"LD E,D","-","-","-","-",1,4,4,0x5A},
  {"LD E,E","-","-","-","-",1,4,4,0x5B},
  {"LD E,H","-","-","-","-",1,4,4,0x5C},
  {"LD E,L","-","-","-","-",1,4,4,0x5D},
  {"LD E,(HL)","-","-","-","-",1,8,8,0x5E},
  {"LD E,A","-","-","-","-",1,4,4,0x5F},
  {"LD H,B","-","-","-","-",1,4,4,0x60},
  {"LD H,C","-","-","-","-",1,4,4,0x61},
  {"LD H,D","-","-","-","-",1,4,4,0x62},
  {"LD H,E","-","-","-","-",1,4,4,0x63},
  {"LD H,H","-","-","-","-",1,4,4,0x64},
  {"LD H,L","-","-","-","-",1,4,4,0x65},
  {"LD H,(HL)","-","-","-","-",1,8,8,0x66},
  {"LD H,A","-","-","-","-",1,4,4,0x67},
  {"LD L,B","-","-","-","-",1,4,4,0x68},
  {"LD L,C","-","-","-","-",1,4,4,0x69},
  {"LD L,D","-","-","-","-",1,4,4,0x6A},
  {"LD L,E","-","-","-","-",1,4,4,0x6B},
  {"LD L,H","-","-","-","-",1,4,4,0x6C},
  {"LD L,L","-","-","-","-",1,4,4,0x6D},
  {"LD L,(HL)","-","-","-","-",1,8,8,0x6E},
  {"LD L,A","-","-","-","-",1,4,4,0x6F},
  {"LD (HL),B","-","-","-","-",1,8,8,0x70},
  {"LD (HL),C","-","-","-","-",1,8,8,0x71},
  {"LD (HL),D","-","-","-","-",1,8,8,0x72},
  {"LD (HL),E","-","-","-","-",1,8,8,0x73},
  {"LD (HL),H","-","-","-","-",1,8,8,0x74},
  {"LD (HL),L","-","-","-","-",1,8,8,0x75},
  {"HALT","-","-","-","-",1,4,4,0x76},
  {"LD (HL),A","-","-","-","-",1,8,8,0x77},
  {"LD A,B","-","-","-","-",1,4,4,0x78},
  {"LD A,C","-","-","-","-",1,4,4,0x79},
  {"LD A,D","-","-","-","-",1,4,4,0x7A},
  {"LD A,E","-","-","-","-",1,4,4,0x7B},
  {"LD A,H","-","-","-","-",1,4,4,0x7C},
  {"LD A,L","-","-","-","-",1,4,4,0x7D},
  {"LD A,(HL)","-","-","-","-",1,8,8,0x7E},
  {"LD A,A","-","-","-","-",1,4,4,0x7F},
  {"ADD A,B","Z","0","H","C",1,4,4,0x80},
  {"ADD A,C","Z","0","H","C",1,4,4,0x81},
  {"ADD A,D","Z","0","H","C",1,4,4,0x82},
  {"ADD A,E","Z","0","H","C",1,4,4,0x83},
  {"ADD A,H","Z","0","H","C",1,4,4,0x84},
  {"ADD A,L","Z","0","H","C",1,4,4,0x85},
  {"ADD A,(HL)","Z","0","H","C",1,8,8,0x86},
  {"ADD A,A","Z","0","H","C",1,4,4,0x87},
  {"ADC A,B","Z","0","H","C",1,4,4,0x88},
  {"ADC A,C","Z","0","H","C",1,4,4,0x89},
  {"ADC A,D","Z","0","H","C",1,4,4,0x8A},
  {"ADC A,E","Z","0","H","C",1,4,4,0x8B},
  {"ADC A,H","Z","0","H","C",1,4,4,0x8C},
  {"ADC A,L","Z","0","H","C",1,4,4,0x8D},
  {"ADC A,(HL)","Z","0","H","C",1,8,8,0x8E},
  {"ADC A,A","Z","0","H","C",1,4,4,0x8F},
  {"SUB B","Z","1","H","C",1,4,4,0x90},
  {"SUB C","Z","1","H","C",1,4,4,0x91},
  {"SUB D","Z","1","H","C",1,4,4,0x92},
  {"SUB E","Z","1","H","C",1,4,4,0x93},
  {"SUB H","Z","1","H","C",1,4,4,0x94},
  {"SUB L","Z","1","H","C",1,4,4,0x95},
  {"SUB (HL)","Z","1","H","C",1,8,8,0x96},
  {"SUB A","Z","1","H","C",1,4,4,0x97},
  {"SBC A,B","Z","1","H","C",1,4,4,0x98},
  {"SBC A,C","Z","1","H","C",1,4,4,0x99},
  {"SBC A,D","Z","1","H","C",1,4,4,0x9A},
  {"SBC A,E","Z","1","H","C",1,4,4,0x9B},
  {"SBC A,H","Z","1","H","C",1,4,4,0x9C},
  {"SBC A,L","Z","1","H","C",1,4,4,0x9D},
  {"SBC A,(HL)","Z","1","H","C",1,8,8,0x9E},
  {"SBC A,A","Z","1","H","C",1,4,4,0x9F},
  {"AND B","Z","0","1","0",1,4,4,0xA0},
  {"AND C","Z","0","1","0",1,4,4,0xA1},
  {"AND D","Z","0","1","0",1,4,4,0xA2},
  {"AND E","Z","0","1","0",1,4,4,0xA3},
  {"AND H","Z","0","1","0",1,4,4,0xA4},
  {"AND L","Z","0","1","0",1,4,4,0xA5},
  {"AND (HL)","Z","0","1","0",1,8,8,0xA6},
  {"AND A","Z","0","1","0",1,4,4,0xA7},
  {"XOR B","Z","0","0","0",1,4,4,0xA8},
  {"XOR C","Z","0","0","0",1,4,4,0xA9},
  {"XOR D","Z","0","0","0",1,4,4,0xAA},
  {"XOR E","Z","0","0","0",1,4,4,0xAB},
  {"XOR H","Z","0","0","0",1,4,4,0xAC},
  {"XOR L","Z","0","0","0",1,4,4,0xAD},
  {"XOR (HL)","Z","0","0","0",1,8,8,0xAE},
  {"XOR A","Z","0","0","0",1,4,4,0xAF},
  {"OR B","Z","0","0","0",1,4,4,0xB0},
  {"OR C","Z","0","0","0",1,4,4,0xB1},
  {"OR D","Z","0","0","0",1,4,4,0xB2},
  {"OR E","Z","0","0","0",1,4,4,0xB3},
  {"OR H","Z","0","0","0",1,4,4,0xB4},
  {"OR L","Z","0","0","0",1,4,4,0xB5},
  {"OR (HL)","Z","0","0","0",1,8,8,0xB6},
  {"OR A","Z","0","0","0",1,4,4,0xB7},
  {"CP B","Z","1","H","C",1,4,4,0xB8},
  {"CP C","Z","1","H","C",1,4,4,0xB9},
  {"CP D","Z","1","H","C",1,4,4,0xBA},
  {"CP E","Z","1","H","C",1,4,4,0xBB},
  {"CP H","Z","1","H","C",1,4,4,0xBC},
  {"CP L","Z","1","H","C",1,4,4,0xBD},
  {"CP (HL)","Z","1","H","C",1,8,8,0xBE},
  {"CP A","Z","1","H","C",1,4,4,0xBF},
  {"RET NZ","-","-","-","-",1,20,8,0xC0},
  {"POP BC","-","-","-","-",1,12,12,0xC1},
  {"JP NZ,a16","-","-","-","-",3,16,12,0xC2},
  {"JP a16","-","-","-","-",3,16,16,0xC3},
  {"CALL NZ,a16","-","-","-","-",3,24,12,0xC4},
  {"PUSH BC","-","-","-","-",1,16,16,0xC5},
  {"ADD A,d8","Z","0","H","C",2,8,8,0xC6},
  {"RST 00H","-","-","-","-",1,16,16,0xC7},
  {"RET Z","-","-","-","-",1,20,8,0xC8},
  {"RET","-","-","-","-",1,16,16,0xC9},
  {"JP Z,a16","-","-","-","-",3,16,12,0xCA},
  {"PREFIX CB","-","-","-","-",1,4,4,0xCB},
  {"CALL Z,a16","-","-","-","-",3,24,12,0xCC},
  {"CALL a16","-","-","-","-",3,24,24,0xCD},
  {"ADC A,d8","Z","0","H","C",2,8,8,0xCE},
  {"RST 08H","-","-","-","-",1,16,16,0xCF},
  {"RET NC","-","-","-","-",1,20,8,0xD0},
  {"POP DE","-","-","-","-",1,12,12,0xD1},
  {"JP NC,a16","-","-","-","-",3,16,12,0xD2},
  {"invalid","","u","u","u",0,0,0,0xD3},
  {"CALL NC,a16","-","-","-","-",3,24,12,0xD4},
  {"PUSH DE","-","-","-","-",1,16,16,0xD5},
  {"SUB d8","Z","1","H","C",2,8,8,0xD6},
  {"RST 10H","-","-","-","-",1,16,16,0xD7},
  {"RET C","-","-","-","-",1,20,8,0xD8},
  {"RETI","-","-","-","-",1,16,16,0xD9},
  {"JP C,a16","-","-","-","-",3,16,12,0xDA},
  {"invalid","","u","u","u",0,0,0,0xDB},
  {"CALL C,a16","-","-","-","-",3,24,12,0xDC},
  {"invalid","","u","u","u",0,0,0,0xDD},
  {"SBC A,d8","Z","1","H","C",2,8,8,0xDE},
  {"RST 18H","-","-","-","-",1,16,16,0xDF},
  {"LDH (a8),A","-","-","-","-",2,12,12,0xE0},
  {"POP HL","-","-","-","-",1,12,12,0xE1},
  {"LD (C),A","-","-","-","-",2,8,8,0xE2},
  {"invalid","","u","u","u",0,0,0,0xE3},
  {"invalid","","u","u","u",0,0,0,0xE4},
  {"PUSH HL","-","-","-","-",1,16,16,0xE5},
  {"AND d8","Z","0","1","0",2,8,8,0xE6},
  {"RST 20H","-","-","-","-",1,16,16,0xE7},
  {"ADD SP,r8","0","0","H","C",2,16,16,0xE8},
  {"JP (HL)","-","-","-","-",1,4,4,0xE9},
  {"LD (a16),A","-","-","-","-",3,16,16,0xEA},
  {"invalid","","u","u","u",0,0,0,0xEB},
  {"invalid","","u","u","u",0,0,0,0xEC},
  {"invalid","","u","u","u",0,0,0,0xED},
  {"XOR d8","Z","0","0","0",2,8,8,0xEE},
  {"RST 28H","-","-","-","-",1,16,16,0xEF},
  {"LDH A,(a8)","-","-","-","-",2,12,12,0xF0},
  {"POP AF","Z","N","H","C",1,12,12,0xF1},
  {"LD A,(C)","-","-","-","-",2,8,8,0xF2},
  {"DI","-","-","-","-",1,4,4,0xF3},
  {"invalid","","u","u","u",0,0,0,0xF4},
  {"PUSH AF","-","-","-","-",1,16,16,0xF5},
  {"OR d8","Z","0","0","0",2,8,8,0xF6},
  {"RST 30H","-","-","-","-",1,16,16,0xF7},
  {"LD HL,SP+r8","0","0","H","C",2,12,12,0xF8},
  {"LD SP,HL","-","-","-","-",1,8,8,0xF9},
  {"LD A,(a16)","-","-","-","-",3,16,16,0xFA},
  {"EI","-","-","-","-",1,4,4,0xFB},
  {"invalid","","u","u","u",0,0,0,0xFC},
  {"invalid","","u","u","u",0,0,0,0xFD},
  {"CP d8","Z","1","H","C",2,8,8,0xFE},
  {"RST 38H","-","-","-","-",1,16,16,0xFF}
};

node cb[] = {
  {"RLC B","Z","0","0","C",2,8,8,0x0},
  {"RLC C","Z","0","0","C",2,8,8,0x1},
  {"RLC D","Z","0","0","C",2,8,8,0x2},
  {"RLC E","Z","0","0","C",2,8,8,0x3},
  {"RLC H","Z","0","0","C",2,8,8,0x4},
  {"RLC L","Z","0","0","C",2,8,8,0x5},
  {"RLC (HL)","Z","0","0","C",2,16,16,0x6},
  {"RLC A","Z","0","0","C",2,8,8,0x7},
  {"RRC B","Z","0","0","C",2,8,8,0x8},
  {"RRC C","Z","0","0","C",2,8,8,0x9},
  {"RRC D","Z","0","0","C",2,8,8,0xA},
  {"RRC E","Z","0","0","C",2,8,8,0xB},
  {"RRC H","Z","0","0","C",2,8,8,0xC},
  {"RRC L","Z","0","0","C",2,8,8,0xD},
  {"RRC (HL)","Z","0","0","C",2,16,16,0xE},
  {"RRC A","Z","0","0","C",2,8,8,0xF},
  {"RL B","Z","0","0","C",2,8,8,0x10},
  {"RL C","Z","0","0","C",2,8,8,0x11},
  {"RL D","Z","0","0","C",2,8,8,0x12},
  {"RL E","Z","0","0","C",2,8,8,0x13},
  {"RL H","Z","0","0","C",2,8,8,0x14},
  {"RL L","Z","0","0","C",2,8,8,0x15},
  {"RL (HL)","Z","0","0","C",2,16,16,0x16},
  {"RL A","Z","0","0","C",2,8,8,0x17},
  {"RR B","Z","0","0","C",2,8,8,0x18},
  {"RR C","Z","0","0","C",2,8,8,0x19},
  {"RR D","Z","0","0","C",2,8,8,0x1A},
  {"RR E","Z","0","0","C",2,8,8,0x1B},
  {"RR H","Z","0","0","C",2,8,8,0x1C},
  {"RR L","Z","0","0","C",2,8,8,0x1D},
  {"RR (HL)","Z","0","0","C",2,16,16,0x1E},
  {"RR A","Z","0","0","C",2,8,8,0x1F},
  {"SLA B","Z","0","0","C",2,8,8,0x20},
  {"SLA C","Z","0","0","C",2,8,8,0x21},
  {"SLA D","Z","0","0","C",2,8,8,0x22},
  {"SLA E","Z","0","0","C",2,8,8,0x23},
  {"SLA H","Z","0","0","C",2,8,8,0x24},
  {"SLA L","Z","0","0","C",2,8,8,0x25},
  {"SLA (HL)","Z","0","0","C",2,16,16,0x26},
  {"SLA A","Z","0","0","C",2,8,8,0x27},
  {"SRA B","Z","0","0","0",2,8,8,0x28},
  {"SRA C","Z","0","0","0",2,8,8,0x29},
  {"SRA D","Z","0","0","0",2,8,8,0x2A},
  {"SRA E","Z","0","0","0",2,8,8,0x2B},
  {"SRA H","Z","0","0","0",2,8,8,0x2C},
  {"SRA L","Z","0","0","0",2,8,8,0x2D},
  {"SRA (HL)","Z","0","0","0",2,16,16,0x2E},
  {"SRA A","Z","0","0","0",2,8,8,0x2F},
  {"SWAP B","Z","0","0","0",2,8,8,0x30},
  {"SWAP C","Z","0","0","0",2,8,8,0x31},
  {"SWAP D","Z","0","0","0",2,8,8,0x32},
  {"SWAP E","Z","0","0","0",2,8,8,0x33},
  {"SWAP H","Z","0","0","0",2,8,8,0x34},
  {"SWAP L","Z","0","0","0",2,8,8,0x35},
  {"SWAP (HL)","Z","0","0","0",2,16,16,0x36},
  {"SWAP A","Z","0","0","0",2,8,8,0x37},
  {"SRL B","Z","0","0","C",2,8,8,0x38},
  {"SRL C","Z","0","0","C",2,8,8,0x39},
  {"SRL D","Z","0","0","C",2,8,8,0x3A},
  {"SRL E","Z","0","0","C",2,8,8,0x3B},
  {"SRL H","Z","0","0","C",2,8,8,0x3C},
  {"SRL L","Z","0","0","C",2,8,8,0x3D},
  {"SRL (HL)","Z","0","0","C",2,16,16,0x3E},
  {"SRL A","Z","0","0","C",2,8,8,0x3F},
  {"BIT 0,B","Z","0","1","-",2,8,8,0x40},
  {"BIT 0,C","Z","0","1","-",2,8,8,0x41},
  {"BIT 0,D","Z","0","1","-",2,8,8,0x42},
  {"BIT 0,E","Z","0","1","-",2,8,8,0x43},
  {"BIT 0,H","Z","0","1","-",2,8,8,0x44},
  {"BIT 0,L","Z","0","1","-",2,8,8,0x45},
  {"BIT 0,(HL)","Z","0","1","-",2,16,16,0x46},
  {"BIT 0,A","Z","0","1","-",2,8,8,0x47},
  {"BIT 1,B","Z","0","1","-",2,8,8,0x48},
  {"BIT 1,C","Z","0","1","-",2,8,8,0x49},
  {"BIT 1,D","Z","0","1","-",2,8,8,0x4A},
  {"BIT 1,E","Z","0","1","-",2,8,8,0x4B},
  {"BIT 1,H","Z","0","1","-",2,8,8,0x4C},
  {"BIT 1,L","Z","0","1","-",2,8,8,0x4D},
  {"BIT 1,(HL)","Z","0","1","-",2,16,16,0x4E},
  {"BIT 1,A","Z","0","1","-",2,8,8,0x4F},
  {"BIT 2,B","Z","0","1","-",2,8,8,0x50},
  {"BIT 2,C","Z","0","1","-",2,8,8,0x51},
  {"BIT 2,D","Z","0","1","-",2,8,8,0x52},
  {"BIT 2,E","Z","0","1","-",2,8,8,0x53},
  {"BIT 2,H","Z","0","1","-",2,8,8,0x54},
  {"BIT 2,L","Z","0","1","-",2,8,8,0x55},
  {"BIT 2,(HL)","Z","0","1","-",2,16,16,0x56},
  {"BIT 2,A","Z","0","1","-",2,8,8,0x57},
  {"BIT 3,B","Z","0","1","-",2,8,8,0x58},
  {"BIT 3,C","Z","0","1","-",2,8,8,0x59},
  {"BIT 3,D","Z","0","1","-",2,8,8,0x5A},
  {"BIT 3,E","Z","0","1","-",2,8,8,0x5B},
  {"BIT 3,H","Z","0","1","-",2,8,8,0x5C},
  {"BIT 3,L","Z","0","1","-",2,8,8,0x5D},
  {"BIT 3,(HL)","Z","0","1","-",2,16,16,0x5E},
  {"BIT 3,A","Z","0","1","-",2,8,8,0x5F},
  {"BIT 4,B","Z","0","1","-",2,8,8,0x60},
  {"BIT 4,C","Z","0","1","-",2,8,8,0x61},
  {"BIT 4,D","Z","0","1","-",2,8,8,0x62},
  {"BIT 4,E","Z","0","1","-",2,8,8,0x63},
  {"BIT 4,H","Z","0","1","-",2,8,8,0x64},
  {"BIT 4,L","Z","0","1","-",2,8,8,0x65},
  {"BIT 4,(HL)","Z","0","1","-",2,16,16,0x66},
  {"BIT 4,A","Z","0","1","-",2,8,8,0x67},
  {"BIT 5,B","Z","0","1","-",2,8,8,0x68},
  {"BIT 5,C","Z","0","1","-",2,8,8,0x69},
  {"BIT 5,D","Z","0","1","-",2,8,8,0x6A},
  {"BIT 5,E","Z","0","1","-",2,8,8,0x6B},
  {"BIT 5,H","Z","0","1","-",2,8,8,0x6C},
  {"BIT 5,L","Z","0","1","-",2,8,8,0x6D},
  {"BIT 5,(HL)","Z","0","1","-",2,16,16,0x6E},
  {"BIT 5,A","Z","0","1","-",2,8,8,0x6F},
  {"BIT 6,B","Z","0","1","-",2,8,8,0x70},
  {"BIT 6,C","Z","0","1","-",2,8,8,0x71},
  {"BIT 6,D","Z","0","1","-",2,8,8,0x72},
  {"BIT 6,E","Z","0","1","-",2,8,8,0x73},
  {"BIT 6,H","Z","0","1","-",2,8,8,0x74},
  {"BIT 6,L","Z","0","1","-",2,8,8,0x75},
  {"BIT 6,(HL)","Z","0","1","-",2,16,16,0x76},
  {"BIT 6,A","Z","0","1","-",2,8,8,0x77},
  {"BIT 7,B","Z","0","1","-",2,8,8,0x78},
  {"BIT 7,C","Z","0","1","-",2,8,8,0x79},
  {"BIT 7,D","Z","0","1","-",2,8,8,0x7A},
  {"BIT 7,E","Z","0","1","-",2,8,8,0x7B},
  {"BIT 7,H","Z","0","1","-",2,8,8,0x7C},
  {"BIT 7,L","Z","0","1","-",2,8,8,0x7D},
  {"BIT 7,(HL)","Z","0","1","-",2,16,16,0x7E},
  {"BIT 7,A","Z","0","1","-",2,8,8,0x7F},
  {"RES 0,B","-","-","-","-",2,8,8,0x80},
  {"RES 0,C","-","-","-","-",2,8,8,0x81},
  {"RES 0,D","-","-","-","-",2,8,8,0x82},
  {"RES 0,E","-","-","-","-",2,8,8,0x83},
  {"RES 0,H","-","-","-","-",2,8,8,0x84},
  {"RES 0,L","-","-","-","-",2,8,8,0x85},
  {"RES 0,(HL)","-","-","-","-",2,16,16,0x86},
  {"RES 0,A","-","-","-","-",2,8,8,0x87},
  {"RES 1,B","-","-","-","-",2,8,8,0x88},
  {"RES 1,C","-","-","-","-",2,8,8,0x89},
  {"RES 1,D","-","-","-","-",2,8,8,0x8A},
  {"RES 1,E","-","-","-","-",2,8,8,0x8B},
  {"RES 1,H","-","-","-","-",2,8,8,0x8C},
  {"RES 1,L","-","-","-","-",2,8,8,0x8D},
  {"RES 1,(HL)","-","-","-","-",2,16,16,0x8E},
  {"RES 1,A","-","-","-","-",2,8,8,0x8F},
  {"RES 2,B","-","-","-","-",2,8,8,0x90},
  {"RES 2,C","-","-","-","-",2,8,8,0x91},
  {"RES 2,D","-","-","-","-",2,8,8,0x92},
  {"RES 2,E","-","-","-","-",2,8,8,0x93},
  {"RES 2,H","-","-","-","-",2,8,8,0x94},
  {"RES 2,L","-","-","-","-",2,8,8,0x95},
  {"RES 2,(HL)","-","-","-","-",2,16,16,0x96},
  {"RES 2,A","-","-","-","-",2,8,8,0x97},
  {"RES 3,B","-","-","-","-",2,8,8,0x98},
  {"RES 3,C","-","-","-","-",2,8,8,0x99},
  {"RES 3,D","-","-","-","-",2,8,8,0x9A},
  {"RES 3,E","-","-","-","-",2,8,8,0x9B},
  {"RES 3,H","-","-","-","-",2,8,8,0x9C},
  {"RES 3,L","-","-","-","-",2,8,8,0x9D},
  {"RES 3,(HL)","-","-","-","-",2,16,16,0x9E},
  {"RES 3,A","-","-","-","-",2,8,8,0x9F},
  {"RES 4,B","-","-","-","-",2,8,8,0xA0},
  {"RES 4,C","-","-","-","-",2,8,8,0xA1},
  {"RES 4,D","-","-","-","-",2,8,8,0xA2},
  {"RES 4,E","-","-","-","-",2,8,8,0xA3},
  {"RES 4,H","-","-","-","-",2,8,8,0xA4},
  {"RES 4,L","-","-","-","-",2,8,8,0xA5},
  {"RES 4,(HL)","-","-","-","-",2,16,16,0xA6},
  {"RES 4,A","-","-","-","-",2,8,8,0xA7},
  {"RES 5,B","-","-","-","-",2,8,8,0xA8},
  {"RES 5,C","-","-","-","-",2,8,8,0xA9},
  {"RES 5,D","-","-","-","-",2,8,8,0xAA},
  {"RES 5,E","-","-","-","-",2,8,8,0xAB},
  {"RES 5,H","-","-","-","-",2,8,8,0xAC},
  {"RES 5,L","-","-","-","-",2,8,8,0xAD},
  {"RES 5,(HL)","-","-","-","-",2,16,16,0xAE},
  {"RES 5,A","-","-","-","-",2,8,8,0xAF},
  {"RES 6,B","-","-","-","-",2,8,8,0xB0},
  {"RES 6,C","-","-","-","-",2,8,8,0xB1},
  {"RES 6,D","-","-","-","-",2,8,8,0xB2},
  {"RES 6,E","-","-","-","-",2,8,8,0xB3},
  {"RES 6,H","-","-","-","-",2,8,8,0xB4},
  {"RES 6,L","-","-","-","-",2,8,8,0xB5},
  {"RES 6,(HL)","-","-","-","-",2,16,16,0xB6},
  {"RES 6,A","-","-","-","-",2,8,8,0xB7},
  {"RES 7,B","-","-","-","-",2,8,8,0xB8},
  {"RES 7,C","-","-","-","-",2,8,8,0xB9},
  {"RES 7,D","-","-","-","-",2,8,8,0xBA},
  {"RES 7,E","-","-","-","-",2,8,8,0xBB},
  {"RES 7,H","-","-","-","-",2,8,8,0xBC},
  {"RES 7,L","-","-","-","-",2,8,8,0xBD},
  {"RES 7,(HL)","-","-","-","-",2,16,16,0xBE},
  {"RES 7,A","-","-","-","-",2,8,8,0xBF},
  {"SET 0,B","-","-","-","-",2,8,8,0xC0},
  {"SET 0,C","-","-","-","-",2,8,8,0xC1},
  {"SET 0,D","-","-","-","-",2,8,8,0xC2},
  {"SET 0,E","-","-","-","-",2,8,8,0xC3},
  {"SET 0,H","-","-","-","-",2,8,8,0xC4},
  {"SET 0,L","-","-","-","-",2,8,8,0xC5},
  {"SET 0,(HL)","-","-","-","-",2,16,16,0xC6},
  {"SET 0,A","-","-","-","-",2,8,8,0xC7},
  {"SET 1,B","-","-","-","-",2,8,8,0xC8},
  {"SET 1,C","-","-","-","-",2,8,8,0xC9},
  {"SET 1,D","-","-","-","-",2,8,8,0xCA},
  {"SET 1,E","-","-","-","-",2,8,8,0xCB},
  {"SET 1,H","-","-","-","-",2,8,8,0xCC},
  {"SET 1,L","-","-","-","-",2,8,8,0xCD},
  {"SET 1,(HL)","-","-","-","-",2,16,16,0xCE},
  {"SET 1,A","-","-","-","-",2,8,8,0xCF},
  {"SET 2,B","-","-","-","-",2,8,8,0xD0},
  {"SET 2,C","-","-","-","-",2,8,8,0xD1},
  {"SET 2,D","-","-","-","-",2,8,8,0xD2},
  {"SET 2,E","-","-","-","-",2,8,8,0xD3},
  {"SET 2,H","-","-","-","-",2,8,8,0xD4},
  {"SET 2,L","-","-","-","-",2,8,8,0xD5},
  {"SET 2,(HL)","-","-","-","-",2,16,16,0xD6},
  {"SET 2,A","-","-","-","-",2,8,8,0xD7},
  {"SET 3,B","-","-","-","-",2,8,8,0xD8},
  {"SET 3,C","-","-","-","-",2,8,8,0xD9},
  {"SET 3,D","-","-","-","-",2,8,8,0xDA},
  {"SET 3,E","-","-","-","-",2,8,8,0xDB},
  {"SET 3,H","-","-","-","-",2,8,8,0xDC},
  {"SET 3,L","-","-","-","-",2,8,8,0xDD},
  {"SET 3,(HL)","-","-","-","-",2,16,16,0xDE},
  {"SET 3,A","-","-","-","-",2,8,8,0xDF},
  {"SET 4,B","-","-","-","-",2,8,8,0xE0},
  {"SET 4,C","-","-","-","-",2,8,8,0xE1},
  {"SET 4,D","-","-","-","-",2,8,8,0xE2},
  {"SET 4,E","-","-","-","-",2,8,8,0xE3},
  {"SET 4,H","-","-","-","-",2,8,8,0xE4},
  {"SET 4,L","-","-","-","-",2,8,8,0xE5},
  {"SET 4,(HL)","-","-","-","-",2,16,16,0xE6},
  {"SET 4,A","-","-","-","-",2,8,8,0xE7},
  {"SET 5,B","-","-","-","-",2,8,8,0xE8},
  {"SET 5,C","-","-","-","-",2,8,8,0xE9},
  {"SET 5,D","-","-","-","-",2,8,8,0xEA},
  {"SET 5,E","-","-","-","-",2,8,8,0xEB},
  {"SET 5,H","-","-","-","-",2,8,8,0xEC},
  {"SET 5,L","-","-","-","-",2,8,8,0xED},
  {"SET 5,(HL)","-","-","-","-",2,16,16,0xEE},
  {"SET 5,A","-","-","-","-",2,8,8,0xEF},
  {"SET 6,B","-","-","-","-",2,8,8,0xF0},
  {"SET 6,C","-","-","-","-",2,8,8,0xF1},
  {"SET 6,D","-","-","-","-",2,8,8,0xF2},
  {"SET 6,E","-","-","-","-",2,8,8,0xF3},
  {"SET 6,H","-","-","-","-",2,8,8,0xF4},
  {"SET 6,L","-","-","-","-",2,8,8,0xF5},
  {"SET 6,(HL)","-","-","-","-",2,16,16,0xF6},
  {"SET 6,A","-","-","-","-",2,8,8,0xF7},
  {"SET 7,B","-","-","-","-",2,8,8,0xF8},
  {"SET 7,C","-","-","-","-",2,8,8,0xF9},
  {"SET 7,D","-","-","-","-",2,8,8,0xFA},
  {"SET 7,E","-","-","-","-",2,8,8,0xFB},
  {"SET 7,H","-","-","-","-",2,8,8,0xFC},
  {"SET 7,L","-","-","-","-",2,8,8,0xFD},
  {"SET 7,(HL)","-","-","-","-",2,16,16,0xFE},
  {"SET 7,A","-","-","-","-",2,8,8,0xFF},
};





















  char *line;
  const char s[2] = "\n";
  char *saveptr1;
  line = strtok_r(rawData, s, &saveptr1);
  while( line != NULL )
  {
    process(line);
    line = strtok_r(NULL, s, &saveptr1);
  }
  /*
  printf("\ndumping label table\n");
  unsigned int i = 0;
  for(i=0;i<label_tbl_n;i++)
  {
    printf("%s\n", label_tbl[i]);
  }
  */
	
	free(src_filename_buffer);
	free(rawData);	
	return 0;
}


int cpu_nop ()
{
	printf("nop\n");
	return 0;
}
int cpu_ld (char *p0, char *p1, char *p2)
{
	long int lp1 = strtol(p1, NULL, 0);
	printf("ld %s, %s, %s\n", p0, p1, p2);

	if(strcmp(p0, "af") == 0) {
		cpureg.AF = lp1;
	}
	else if(strcmp(p0, "bc") == 0) {
		cpureg.BC = lp1;
	}
	else if(strcmp(p0, "de") == 0) {
		cpureg.DE = lp1;
	}
	else if(strcmp(p0, "hl") == 0) {
		cpureg.HL = lp1;
	}
	else if(strcmp(p0, "a") == 0) {
		cpureg.A = lp1;
	}
	else if(strcmp(p0, "f") == 0) {
		cpureg.F = lp1;
	}
	else if(strcmp(p0, "b") == 0) {
		cpureg.B = lp1;
	}
	else if(strcmp(p0, "c") == 0) {
		cpureg.C = lp1;
	}
	else if(strcmp(p0, "d") == 0) {
		cpureg.D = lp1;
	}
	else if(strcmp(p0, "e") == 0) {
		cpureg.E = lp1;
	}
	else if(strcmp(p0, "h") == 0) {
		cpureg.H = lp1;
	}
	else if(strcmp(p0, "l") == 0) {
		cpureg.L = lp1;
	}
	else
	{
		printf("not coded yet\n");
		return -1;
	}
	printf("\t\t\t\tregs\taf\tbc\tde\thl\n\t\t\t\t\t%02hhx %02hhx\t%02hhx %02hhx\t%02hhx %02hhx\t%02hhx %02hhx\n", cpureg.A, cpureg.F, cpureg.B, cpureg.C,  cpureg.D, cpureg.E, cpureg.H, cpureg.L);
	return 0;
}
int cpu_inc (char *p0)
{
	printf("inc %s\n", p0);
	return 0;
}
int cpu_dec (char *p0)
{
	printf("dec %s\n", p0);
	return 0;
}
int cpu_rlca (char *p0)
{
	printf("rlca %s\n", p0);
	return 0;
}
int cpu_add (char *p0, char *p1)
{
	printf("add %s, %s\n", p0, p1);
	return 0;
}
int cpu_rrca (char *p0)
{
	printf("rrca %s\n", p0);
	return 0;
}
int cpu_stop (char *p0, char *p1)
{
	printf("stop %s, %s\n", p0, p1);
	return 0;
}
int cpu_rla (char *p0)
{
	printf("rla %s\n", p0);
	return 0;
}
int cpu_jr (char *p0, char *p1)
{
	printf("jr %s, %s\n", p0, p1);
	return 0;
}
int cpu_rra (char *p0)
{
	printf("rra %s\n", p0);
	return 0;
}
int cpu_daa (char *p0)
{
	printf("daa %s\n", p0);
	return 0;
}
int cpu_cpl (char *p0)
{
	printf("cpl %s\n", p0);
	return 0;
}
int cpu_scf (char *p0)
{
	printf("scf %s\n", p0);
	return 0;
}
int cpu_ccf (char *p0)
{
	printf("ccf %s\n", p0);
	return 0;
}
int cpu_halt (char *p0)
{
	printf("halt %s\n", p0);
	return 0;
}
int cpu_adc (char *p0, char *p1)
{
	printf("adc %s, %s\n", p0, p1);
	return 0;
}
int cpu_sub (char *p0, char *p1)
{
	printf("sub %s, %s\n", p0, p1);
	return 0;
}
int cpu_sbc (char *p0, char *p1)
{
	printf("sbc %s, %s\n", p0, p1);
	return 0;
}
int cpu_and (char *p0, char *p1)
{
	printf("and %s, %s\n", p0, p1);
	return 0;
}
int cpu_xor (char *p0, char *p1)
{
	printf("xor %s, %s\n", p0, p1);
	return 0;
}
int cpu_or (char *p0, char *p1)
{
	printf("or %s, %s\n", p0, p1);
	return 0;
}
int cpu_cp (char *p0, char *p1)
{
	printf("cp %s, %s\n", p0, p1);
	return 0;
}
int cpu_ret (char *p0)
{
	printf("ret %s\n", p0);
	return 0;
}
int cpu_pop (char *p0)
{
	printf("pop %s\n", p0);
	return 0;
}
int cpu_jp (char *p0, char *p1, char *p2)
{
	printf("jp %s, %s, %s\n", p0, p1, p2);
	return 0;
}
int cpu_call (char *p0, char *p1, char *p2)
{
	printf("call %s, %s, %s\n", p0, p1, p2);
	return 0;
}
int cpu_push (char *p0)
{
	printf("push %s\n", p0);
	return 0;
}
int cpu_rst (char *p0)
{
	printf("rst %s\n", p0);
	return 0;
}
int cpu_prefix (char *p0)
{
	printf("prefix %s\n", p0);
	return 0;
}
int cpu_reti (char *p0)
{
	printf("reti %s\n", p0);
	return 0;
}
int cpu_ldh (char *p0, char *p1)
{
	printf("ldh %s, %s\n", p0, p1);
	return 0;
}
int cpu_di (char *p0)
{
	printf("di %s\n", p0);
	return 0;
}
int cpu_ei (char *p0)
{
	printf("ei %s\n", p0);
	return 0;
}




char *process(char *line)
{
  char *tokens;
  char *saveptr2;
  char *saveptr3;
  const char s[3] = "\t ";


  char *line_cache = malloc(strlen(line)+1);
  memset(line_cache, 0, strlen(line)+1);
  memcpy(line_cache, line, strlen(line));
  
  tokens = strtok_r(line_cache, s, &saveptr2);
  size_t t1len = strlen(tokens);
  char ch = tokens[t1len-1];

  char *command = malloc(strlen(tokens));
  memset(command, 0, strlen(tokens));
  memcpy(command, tokens, strlen(tokens));

  //printf("[");
  if(ch == ':')
  {
    printf("%s\n", tokens);
    label_tbl[label_tbl_n] = malloc(t1len);
    memset(label_tbl[label_tbl_n], 0, t1len);
    memcpy(label_tbl[label_tbl_n],
      tokens,
      t1len-1
    );
    label_tbl_n++;
  } else {
	  ch = tokens[0];
	  if(ch == ';')
	  {
		  //printf("comment");
		  free(command);
		  return 0;
	  }

  }

//
const char s3[2] = ",";
memset(line_cache, 0, strlen(line));
memcpy(line_cache, line+t1len, strlen(line)-t1len);
if(strcmp(line,line_cache) == 0) 
{
	//printf("line and line cache match content");
}
char *p0 = strtok_r(line_cache, s3, &saveptr3);
char *p1 = strtok_r(NULL, s3, &saveptr3);
char *p2 = strtok_r(NULL, s3, &saveptr3);


if(p0 != NULL) p0 += 1;
//if(p1 != NULL) p1 += 1;
//if(p2 != NULL) p2 += 1;

if (strcmp("nop",	command) == 0)	cpu_nop();
if (strcmp("ld",	command) == 0)	cpu_ld(p0,p1,p2);
if (strcmp("inc",	command) == 0)	cpu_inc(p0);
if (strcmp("dec",	command) == 0)	cpu_dec(p0);
if (strcmp("rlca",	command) == 0)	cpu_rlca(p0);
if (strcmp("add",	command) == 0)	cpu_add(p0,p1);
if (strcmp("rrca",	command) == 0)	cpu_rrca(p0);
if (strcmp("stop",	command) == 0)	cpu_stop(p0,p1);
if (strcmp("rla",	command) == 0)	cpu_rla(p0);
if (strcmp("jr",	command) == 0)	cpu_jr(p0,p1);
if (strcmp("rra",	command) == 0)	cpu_rra(p0);
if (strcmp("daa",	command) == 0)	cpu_daa(p0);
if (strcmp("cpl",	command) == 0)	cpu_cpl(p0);
if (strcmp("scf",	command) == 0)	cpu_scf(p0);
if (strcmp("ccf",	command) == 0)	cpu_ccf(p0);
if (strcmp("halt",	command) == 0)	cpu_halt(p0);
if (strcmp("adc",	command) == 0)	cpu_adc(p0,p1);
if (strcmp("sub",	command) == 0)	cpu_sub(p0,p1);
if (strcmp("sbc",	command) == 0)	cpu_sbc(p0,p1);
if (strcmp("and",	command) == 0)	cpu_and(p0,p1);
if (strcmp("xor",	command) == 0)	cpu_xor(p0,p1);
if (strcmp("or",	command) == 0)	cpu_or(p0,p1);
if (strcmp("cp",	command) == 0)	cpu_cp(p0,p1);
if (strcmp("ret",	command) == 0)	cpu_ret(p0);
if (strcmp("pop",	command) == 0)	cpu_pop(p0);
if (strcmp("jp",	command) == 0)	cpu_jp(p0,p1,p2);
if (strcmp("call",	command) == 0)	cpu_call(p0,p1,p2);
if (strcmp("push",	command) == 0)	cpu_push(p0);
if (strcmp("rst",	command) == 0)	cpu_rst(p0);
if (strcmp("prefix",	command) == 0)	cpu_prefix(p0);
if (strcmp("reti",	command) == 0)	cpu_reti(p0);
if (strcmp("ldh",	command) == 0)	cpu_ldh(p0,p1);
if (strcmp("di",	command) == 0)	cpu_di(p0);
if (strcmp("ei",	command) == 0)	cpu_ei(p0);
free(command);
  return line;
}


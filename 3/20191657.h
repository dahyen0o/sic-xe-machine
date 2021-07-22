#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <math.h>
#define MEMORY_MAX 1048576

typedef struct NODE{
	struct NODE *link;
	char data[100];
} NODE;

typedef struct MEMORY{
	char address[6];
	char memory[17][3];
	char ascii[17];
} MEMORY;
MEMORY mem[MEMORY_MAX / 16 + 1]; 

typedef struct HASH{
	struct HASH *link;
	char opcode[3];
	char name[11];
	char format[11];
} HASH;
HASH* hash[20];

typedef struct SYMTAB{
	char label[100]; // symbol
	char opcode[100]; int object; char format[11]; // HASH
	char oprand[100];

	int address;
} SYMTAB;
SYMTAB symtab[500], ssymtab[500]; int symidx;

typedef struct ESTAB{
	char cs[10];
	char name[10];
	int address;
	int length;
} ESTAB;
ESTAB estab[500]; int esidx;

typedef struct P1{
	int object;
	int mn, format;
	int n, i, x, b, p, e;
	//int pc, base;
} P1;
P1 p1[MEMORY_MAX / 16 + 1];

typedef struct P2{
	int A, X, L, PC, B, S, T;
} P2;
P2 table[MEMORY_MAX / 16 + 1];

int progaddr, cslth, csaddr, execaddr;
int bparr[100][2], bpidx;
int cc, i, length;
// 전역변수는 초기화 x!!!

void Trim(char []);
void Addlinkedlist(char []);
void Readhash();
void Inserthash(HASH **, HASH *);

void Help();
int Dir();
void History(char []);
int Dump();
int Dumpstart(char []);
int Dumpend(char [], char []);
int Edit(char [], char[]);
int Fill(char [], char[], char[]);
void Reset();
int Opcodemn(char []);
void Opcodelist();

int Type(char []);
int Assemble(char []);
int Symbol();

int Progadd(char []);
int Loader1(char [], int);
int Loader2(char [], int);
void Loaderprint();
void Bp(char []);
void Bpclear();
void Bpprint();
int Run();
int Run2();

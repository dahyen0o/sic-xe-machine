#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
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

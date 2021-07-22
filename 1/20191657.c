#include "20191657.h"

int main(){
	char comm[100]; // command

	/* 초기화*/
	// 사용자에게 입력을 받기 전 hash table, memory 등을 초기화 해놓는다.

	Readhash(); // initialize hashmap of opcode
	for(int j = 0;j < 5;j++) mem[0].address[j] = '0';
	mem[0].address[5] = '\0';
	for(int i = 1;i < MEMORY_MAX / 16;i++){
		strcpy(mem[i].address, mem[i - 1].address);
		mem[i].address[5] = '\0';
		mem[i].address[4] += 1; 

		if(mem[i].address[4] == ':') mem[i].address[4] = 'A';
		if(mem[i].address[4] > 'F'){
			mem[i].address[4] = '0'; mem[i].address[3]++;
		}
		
		if(mem[i].address[3] == ':') mem[i].address[3] = 'A';
		if(mem[i].address[3] > 'F'){
			mem[i].address[3] = '0'; mem[i].address[2]++;
		}

		if(mem[i].address[2] == ':') mem[i].address[2] = 'A';
		if(mem[i].address[2] > 'F'){
			mem[i].address[2] = '0'; mem[i].address[1]++;
		}

		if(mem[i].address[1] == ':') mem[i].address[1] = 'A';
		if(mem[i].address[1] > 'F'){
			mem[i].address[1] = '0'; mem[i].address[0]++;
		}

		if(mem[i].address[0] > ':') mem[i].address[0] = 'A';
		if(mem[i].address[0] > '1'){
			printf("Code Error\n"); return 0;
		}
		
	} // initialize address of memory in array
	Reset(); // initialize others of memory in array

	/* 명령어 입력받기 */
	// Trim 함수를 이용해 공백을 제거 한 명령어로 알맞은 함수를 호출한다.
	// 함수 호출 후, return값이 1일 때만  오류없이 실행 되었으므로 linkedlist에 명령어를 저장한다.
	// 오류가 생기면 "Error"를 화면에 표시한 후 입력값을 새로 받는다.

	while(1){
		printf("sicsim> "); scanf(" %[^\n]", comm);
		Trim(comm); // erase space

		if(strcmp(comm, "help") == 0 || strcmp(comm, "h") == 0) {
			Addlinkedlist(comm); Help();
		}
		else if(strcmp(comm, "dir") == 0 || strcmp(comm, "d") == 0) {
			if(Dir() == 1) Addlinkedlist(comm);
		}
		else if(strcmp(comm, "quit") == 0 || strcmp(comm, "q") == 0) break;
		else if(strcmp(comm, "history") == 0 || strcmp(comm, "hi") == 0){
			Addlinkedlist(comm); History(comm);
		}
		else if(strcmp(comm, "reset") == 0) {
			Addlinkedlist(comm); Reset();
		}
		else if(strcmp(comm, "opcodelist") == 0) {
			Addlinkedlist(comm); Opcodelist();
		}
		else if(strcmp(comm, "dump") == 0 || strcmp(comm, "du") == 0){
			if(Dump() == 1) Addlinkedlist(comm);
		}
		else{
			// char [] - command sets sliced by ',' or ' '
			char comm1[100] = {0, }, comm2[100] = {0, };
		    char comm3[100] = {0, }, comm4[100] = {0, };
			int i, a, b, c, d;
			for(i = 0, a = 0;i < strlen(comm);a++){
				comm1[a] = comm[i++];
				if(comm1[a] == ' '){comm1[a] = '\0'; break;}
			}
			if(i >= strlen(comm)) comm2[0] = '\0';
			for(b = 0;i < strlen(comm);b++){
				comm2[b] = comm[i++];
				if(comm2[b] == ','){comm2[b] = '\0'; break;}
			}
			if(i >= strlen(comm)) comm3[0] = '\0';
			for(c = 0;i < strlen(comm);c++){
				comm3[c] = comm[i++];
				if(comm3[c] == ','){comm3[c] = '\0'; break;}
			}
			if(i >= strlen(comm)) comm4[0] = '\0';
			for(d = 0;i < strlen(comm);d++){
				comm4[d] = comm[i++];
				if(comm4[d] == ','){comm4[d] = '\0'; break;}
			}
			Trim(comm1); Trim(comm2); Trim(comm3); Trim(comm4);

			if(strcmp(comm1, "opcode") == 0 && strlen(comm2) != 0 && strlen(comm3) == 0) {
				for(i = 0;i < strlen(comm2);i++){
					if(comm2[i] >= 'A' && comm2[i] <= 'Z') continue;
					else break;
				} // mnemonic must be upper alphabet
				if(i == strlen(comm2)) {
					strcat(comm1, " "); strcat(comm1, comm2);
					printf("%s %s\n", comm1, comm2);
					if(Opcodemn(comm2) == 1) Addlinkedlist(comm1);
				}
				else {printf("Error\n"); continue;}	
			}
			else if(strcmp(comm1, "dump") == 0 || strcmp(comm1, "du") == 0){
				for(i = 0;i < strlen(comm2);i++){
					if(comm2[i] >= '0' && comm2[i] <= '9') continue;
					else if(comm2[i] >= 'A' && comm2[i] <= 'F') continue;
					else if(comm2[i] >= 'a' && comm2[i] <= 'f'){
						comm2[i] -= 32; continue;
					} // make down -> upper alphabet
					else break;
				} // check comm2 - 16 bit num
				if(i == strlen(comm2) && strlen(comm3) == 0) {
					strcat(comm1, " "); strcat(comm1, comm2);
					if(Dumpstart(comm2) == 1) Addlinkedlist(comm1);
					// dump [start]
					continue;
				}

				for(i = 0;i < strlen(comm3);i++){
					if(comm3[i] >= '0' && comm3[i] <= '9') continue;
					else if(comm3[i] >= 'A' && comm3[i] <= 'F') continue;
					else if(comm3[i] >= 'a' && comm3[i] <= 'f'){
						comm3[i] -= 32; continue;
					}
					else break;
				}
				if(i == strlen(comm3) && strlen(comm4) == 0){
					strcat(comm1, " "); strcat(comm1, comm2); 
					strcat(comm1, ", "); strcat(comm1, comm3);
					if(Dumpend(comm2, comm3) == 1) Addlinkedlist(comm1);
					// dump [start, end]
					continue;
				}
				else {printf("Error\n"); continue;}	
			}
			else if(strcmp(comm1, "edit") == 0 || strcmp(comm1, "e") == 0){
				if(strlen(comm2) == 0 || strlen(comm3) == 0) {
					printf("Error\n"); continue;
				}
				else if(strlen(comm4) != 0) {printf("Error\n"); continue;}
				else{
					for(i = 0;i < strlen(comm2);i++){
						if(comm2[i] >= '0' && comm2[i] <= '9') continue;
						else if(comm2[i] >= 'A' && comm2[i] <= 'F') continue;
						else if(comm2[i] >= 'a' && comm2[i] <= 'f'){
							comm2[i] -= 32; continue;
						} // make down -> upper alphabet
						else break;
					}
					if(i != strlen(comm2)) {printf("Error\n"); continue;}
					else{
						for(i = 0;i < strlen(comm3);i++){
							if(comm3[i] >= '0' && comm3[i] <= '9') continue;
							else if(comm3[i] >= 'A' && comm3[i] <= 'F') continue;
							else if(comm3[i] >= 'a' && comm3[i] <= 'f'){
								comm3[i] -= 32; continue;
							} // make down -> upper alphabet
							else break;
						}
						if(i == strlen(comm3)){
							strcat(comm1, " "); strcat(comm1, comm2);
							strcat(comm1, ", "); strcat(comm1, comm3);
							if(Edit(comm2, comm3) == 1) Addlinkedlist(comm1);
							continue;
						}
						else {printf("Error\n"); continue;}
					}
				}
			}
			else if(strcmp(comm1, "fill") == 0 || strcmp(comm1, "f") == 0){
				if(strlen(comm2) == 0 || strlen(comm3) == 0 || strlen(comm4) == 0){
					printf("Error\n"); continue;
				}
				else{
					for(i = 0;i < strlen(comm2);i++){
						if(comm2[i] >= '0' && comm2[i] <= '9') continue;
						else if(comm2[i] >= 'A' && comm2[i] <= 'F') continue;
						else if(comm2[i] >= 'a' && comm2[i] <= 'f'){
							comm2[i] -= 32; continue;
						} // make down -> upper alphabet
						else break;
					}
					if(i != strlen(comm2)) {printf("Error\n"); continue;}
					else{
						for(i = 0;i < strlen(comm3);i++){
							if(comm3[i] >= '0' && comm3[i] <= '9') continue;
							else if(comm3[i] >= 'A' && comm3[i] <= 'F') continue;
							else if(comm3[i] >= 'a' && comm3[i] <= 'f'){
								comm3[i] -= 32; continue;
							} // make down -> upper alphabet
							else break;
						}
						if(i != strlen(comm3)) {printf("Error\n"); continue;}
						else{
							for(i = 0;i < strlen(comm4);i++){
								if(comm4[i] >= '0' && comm4[i] <= '9') continue;
								else if(comm4[i] >= 'A' && comm4[i] <= 'F') continue;
								else if(comm4[i] >= 'a' && comm4[i] <= 'f'){
									comm4[i] -= 32; continue;
								} // make down -> upper alphabet
								else break;
							}
							if(i != strlen(comm4)){
								printf("Error\n"); continue;
							}
							else {
								strcat(comm1, " "); strcat(comm1, comm2);
								strcat(comm1, ", "); strcat(comm1, comm3);
								strcat(comm1, ", "); strcat(comm1, comm4);
								if(Fill(comm2, comm3, comm4) == 1)
									Addlinkedlist(comm1); 
							}
						}
					}
				}
			}
			else{printf("Error\n"); continue;} // not-proper command
		}
	}

	return 0;
}

/* Trim */
// command의 앞 뒤 공백을 제거한다.
// input: char command[], return: void

void Trim(char command[]){
	int i, j, len = strlen(command);
	/* front trim */
	for(i = 0;i < len;i++){
		if(command[i] != ' ' && command[i] != '\t') break;
	}
	if(i == len) {command[0] = '\0'; return;}
	for(j = 0;j < len - i;j++) command[j] = command[i + j];
	command[len - i] = '\0';
	
	/* back trim */
	for(i = strlen(command) - 1;;i--){
		if(command[i] != ' ' && command[i] != '\t') break;
	}
	command[++i] = '\0';
}

/* Addlinkedlist */
// History 함수를 위해 명령어 기록을 linkedlist에 저장한다.
// input: char comm[], return: void

void Addlinkedlist(char comm[]){
	NODE *newnode = (NODE *)malloc(sizeof(NODE));
	newnode->link = NULL; strcpy(newnode->data, comm);

	if(head == NULL) {
		head = newnode;
	}
	else{
		NODE *curr = head;
		while(curr->link != NULL) curr = curr->link;
		curr->link = newnode;
	}
}

/* Help */
// 가능한 명령어를 출력한다.
// input, return: void

void Help(){
	printf("h[elp]\nd[ir]\nq[uit]\nhi[story]\n");
	printf("du[mp] [start, end]\ne[dit] address, value\n");
	printf("f[ill] start, end, value\nreset\n");
	printf("opcoe mnemonic\nopcodelist\n\n");
}

/* Dir */
// 현재 디렉토리의 파일 목록을 보여준다.
// input: void, return: 0(error) or 1(no error)

int Dir(){
	DIR* dir = opendir("."); struct dirent* dir_info;
	
	if(dir == NULL){
		printf("Error\n"); return 0;
	}
	else{
		struct stat sb;
		while(1) {
			dir_info = readdir(dir);
			if(!dir_info) break;

			if(stat(dir_info->d_name, &sb) == -1){
				printf("Error\n"); return 0;
			}

			if(S_ISDIR(sb.st_mode)) printf("%s/\n", dir_info->d_name); // determine - directory
			else if(S_ISREG(sb.st_mode)) {
				// determine - executable file
				if(sb.st_mode & S_IXUSR) 
					printf("%s*\n", dir_info->d_name);
				else if(sb.st_mode & S_IXGRP) 
					printf("%s*\n", dir_info->d_name);
				else if(sb.st_mode & S_IXOTH) 
					printf("%s*\n", dir_info->d_name);
				else printf("%s\n", dir_info->d_name);
			}
			else printf("%s\n", dir_info->d_name);
		}
		closedir(dir);
	}
	return 1;
}

/* History */
// 명령어 기록을 차례대로 출력한다.
// input: char comm[], return: void

void History(char comm[]){
	NODE *curr = head; int i = 0;
	for(;curr != NULL;i++){
		printf("%-4d %s\n", i, curr->data); curr = curr->link;
	}	
}

/* Dump */
// dumpidx부터 메모리 열 줄을 출력한다.
// input: void, return: 0 or 1

int Dump(){
	if(dumpidx < 0 || dumpidx >= MEMORY_MAX / 16) {
		printf("Error\n"); return 0;
	}
	else if(dumpidx != 0) dumpidx++;

	for(int count = 0;count < 10;count++, dumpidx++){
		// 주소의 끝까지 가면 출력을 중단한다.
		if(dumpidx == MEMORY_MAX / 16) break;

		printf("%s  ", mem[dumpidx].address);
		for(int i = 0;i < 16;i++){
			printf("%s ", mem[dumpidx].memory[i]);
		}
		printf("; %s\n", mem[dumpidx].ascii);
	}
	if(dumpidx == MEMORY_MAX / 16) dumpidx = 0;
	else dumpidx--;
	return 1;
}

/* Dumpstart */
// dump [start] 명령어를 수행한다.
// input: char start[], return: 0 or 1

int Dumpstart(char start[]){
	if(strlen(start) > 5) {printf("Error\n"); return 0;}
	int i, j, count = 0; int startrow = 0, startcol;

	/* start(16진법) -> startrow(10진법) */
	for(i = strlen(start) - 1, j = 1;i > -1;i--){
		if(start[i] >= 'A' && start[i] <= 'F') 
			startrow += ((int)start[i] - 55) * j;
		else startrow += ((int)start[i] - '0') * j; 
		j *= 16;
	}
	// startrow = mem address, startcol = value address of startrow line
	startcol = startrow % 16; startrow /= 16;
	if(startrow >= MEMORY_MAX / 16) {printf("Error\n"); return 0;}
	
	// count: idx of memory
	for(count = 0;count < 160;startrow++){
		if(startrow >= MEMORY_MAX / 16) break;
		printf("%s  ", mem[startrow].address);

		if(count == 0){
			for(i = 0;i < startcol;i++) printf("   "); // make space
			for(i = startcol;i < 16;i++, count++){
				if(count == 160) break;
				for(j = 0;j < 2;j++){
					printf("%c", mem[startrow].memory[i][j]);
				}
				printf(" ");
			}
			printf("; ");
			for(i = 0;i < startcol;i++) printf(".");
			for(i = startcol;i < 16;i++) printf("%c", mem[startrow].ascii[i]);
			printf("\n");
		} // first line
		else{
			for(i = 0;i < 16;i++, count++){
				if(count >= 160) {
					printf("   "); continue;
				}
				for(j = 0;j < 2;j++){
					printf("%c", mem[startrow].memory[i][j]);
				}
				printf(" ");
			}
			printf("; %s\n", mem[startrow].ascii);
		} // else line
	}

	dumpidx = startrow - 1; // store last address
	if(dumpidx >= MEMORY_MAX) dumpidx = 0;
	return 1;
}

/* Dumpend */
// dump [start, end] 명령어를 수행한다.
// input: char start[], char end[], return 0 or 1

int Dumpend(char start[], char end[]){
	if(strlen(start) > 5 || strlen(end) > 5) {printf("Error\n"); return 0;}
	int i, j; int startrow = 0, startcol, endrow = 0, endcol;

	// 16 -> 10진수
	for(i = strlen(start) - 1, j = 1;i > -1;i--){
		if(start[i] >= 'A' && start[i] <= 'F') 
			startrow += ((int)start[i] - 55) * j;
		else startrow += ((int)start[i] - '0') * j; 
		j *= 16;
	}
	for(i = strlen(end) - 1, j = 1;i > -1;i--){
		if(end[i] >= 'A' && end[i] <= 'F') 
			endrow += ((int)end[i] - 55) * j;
		else endrow += ((int)end[i] - '0') * j; 
		j *= 16;
	}
	if(startrow > endrow) {printf("Error\n"); return 0;}
	startcol = startrow % 16; startrow /= 16;
	endcol = endrow % 16; endrow /= 16;
	if(endrow >= MEMORY_MAX / 16){printf("Error\n"); return 0;}

	// row: idx of row in mem
	for(int row = startrow;row <= endrow;row++){
		printf("%s  ", mem[row].address);
		if(row == startrow){
			if(startrow == endrow){			
				for(i = 0;i < startcol;i++) printf("   ");
				for(i = startcol;i <= endcol;i++)
					printf("%s ", mem[row].memory[i]);
				for(i = endcol + 1;i < 16;i++) printf("   ");

				printf(" ; ");
				for(i = 0;i < startcol;i++) printf(".");
				for(i = startcol;i <= endcol;i++)
					printf("%c", mem[row].ascii[i]);
				for(i = endcol + 1;i < 16;i++) printf(".");
				printf("\n");
				break;
			} // end in one line
			else{
				for(i = 0;i < startcol;i++) printf("   ");
				for(i = startcol;i < 16;i++) printf("%s ", mem[row].memory[i]);
				
				printf(" ; ");
				for(i = 0;i < startcol;i++) printf(".");
				for(i = startcol;i < 16;i++)
					printf("%c", mem[row].ascii[i]);
				printf("\n");
			} // not end in one line
		} // first line
		else if(row == endrow){
			for(i = 0;i <= endcol;i++)
				printf("%s ", mem[row].memory[i]);
			for(i = endcol + 1;i < 16;i++) printf("   ");

			printf(" ; ");
			for(i = 0;i <= endcol;i++)
				printf("%c", mem[row].ascii[i]);
			for(i = endcol + 1;i < 16;i++) printf(".");
			printf("\n");
		} // last line
		else{
			for(i = 0;i < 16;i++) printf("%s ", mem[row].memory[i]);
			
			printf(" ; ");
			for(i = 0;i < 16;i++) printf("%c", mem[row].ascii[i]);
			printf("\n");
		} // middle line
	}

	dumpidx = endrow;
	if(dumpidx == MEMORY_MAX) dumpidx = 0;
	return 1;
}

/* Edit */
// edit address, value 명령어를 수행한다.
// input: char address[], char value[], return 0 or 1

int Edit(char address[], char value[]){
	int i, j, idx = 0, idxrow, idxcol;
	/* address(16진법) -> idx(10진법) */
	for(i = strlen(address) - 1, j = 1;i > -1;i--){
		if(address[i] >= 'A' && address[i] <= 'F') 
			idx += ((int)address[i] - 55) * j;
		else idx += ((int)address[i] - '0') * j; 
		j *= 16;
	}
	if(idx > MEMORY_MAX - 1 || strlen(value) > 2)
	{
		printf("Error\n"); return 0;
	}
	
	idxrow = idx / 16; idxcol = idx % 16; // idx of address & value
	strcpy(mem[idxrow].memory[idxcol], value); // edit value
   	// edit ascii code
	int asciicode = 0;
	if(value[0] >= '0' && value[0] <= '9') 
		asciicode += (value[0] - '0') * 16;
	else asciicode += (value[0] - 55) * 16;
	if(value[1] >= '0' && value[1] <= '9') 
		asciicode += value[1] - '0';
	else asciicode += value[1] - 55;
	mem[idxrow].ascii[idxcol] = asciicode;

	if(mem[idxrow].ascii[idxcol] < 32 || mem[idxrow].ascii[idxcol] > 126)
		mem[idxrow].ascii[idxcol] = '.'; // 20 ~ 7E
	return 1;
}

/* Fill */
// fill start, end, value 명령어를 수행한다.
// input: char start[], char end[], char value[], return: 0 or 1

int Fill(char start[], char end[], char value[]){
	int i, j, startrow = 0, startcol, endrow = 0, endcol;
	/* start, end(16진법) -> startrow, endrow(10진법)*/
	for(i = strlen(start) - 1, j = 1;i > -1;i--){
		if(start[i] >= 'A' && start[i] <= 'F') 
			startrow += ((int)start[i] - 55) * j;
		else startrow += ((int)start[i] - '0') * j; 
		j *= 16;
	}
	for(i = strlen(end) - 1, j = 1;i > -1;i--){
		if(end[i] >= 'A' && end[i] <= 'F') 
			endrow += ((int)end[i] - 55) * j;
		else endrow += ((int)end[i] - '0') * j; 
		j *= 16;
	}
	if(startrow > endrow){printf("Error\n"); return 0;}
	if(endrow > 16 * MEMORY_MAX - 1 || strlen(value) > 2){
		printf("Error\n"); return 0;
	}
	if(strlen(value) == 1){
		value[1] = value[0]; value[0] = '0';
	} // make value format(ex. 0 -> 00, H -> 0H)

	startcol = startrow % 16; startrow /= 16;
	endcol = endrow % 16; endrow /= 16;

	int row = startrow;
	for(;row <= endrow;row++){
		int asciicode = 0;
		if(value[0] >= '0' && value[0] <= '9') 
			asciicode += (value[0] - '0') * 16;
		else asciicode += (value[0] - 55) * 16;
		if(value[1] >= '0' && value[1] <= '9') 
			asciicode += value[1] - '0';
		else asciicode += value[1] - 55;

		if(row == startrow){
			if(startrow == endrow){
				for(i = startcol;i <= endcol;i++){
					strcpy(mem[row].memory[i], value);

					mem[row].ascii[i] = asciicode;
					if(mem[row].ascii[i] < 32 || mem[row].ascii[i] > 126)
						mem[row].ascii[i] = '.';
				}
			} // 한 줄로 끝남
			else{
				for(i = startcol;i < 16;i++){
					strcpy(mem[row].memory[i], value);

					mem[row].ascii[i] = asciicode;
					if(mem[row].ascii[i] < 32 || mem[row].ascii[i] > 126)
						mem[row].ascii[i] = '.';
				}
			} 
		} // first line
		else if(row == endrow){
			for(i = 0;i <= endcol;i++){
				strcpy(mem[row].memory[i], value);

				mem[row].ascii[i] = asciicode;
				if(mem[row].ascii[i] < 32 || mem[row].ascii[i] > 126)
					mem[row].ascii[i] = '.';
			}
		} // end line
		else{
			for(i = 0;i < 16;i++){
				strcpy(mem[row].memory[i], value);

				mem[row].ascii[i] = asciicode;
				if(mem[row].ascii[i] < 32 || mem[row].ascii[i] > 126)
					mem[row].ascii[i] = '.';
			}
		} // middle line
	}
	return 1;
}

/* Reset */
// reset 명령어를 수행한다.
// input, return: void

void Reset(){
	for(int i = 0;i < MEMORY_MAX / 16;i++){
		for(int j = 0;j < 16;j++){
			for(int k = 0;k < 2;k++) mem[i].memory[j][k] = '0';
			mem[i].ascii[j] = '.';
		}
	}
}

/* Readhash */
// main 함수의 처음에서 hash table의 초기화 및 read를 수행한다.
// input, return: void

void Readhash(){
	// file input
	FILE *fp = fopen("opcode.txt", "r");
	if(fp == NULL) {printf("Code error\n"); return;}
	for(int i = 0;i < 20;i++) {
		hash[i] = NULL;
	}

	// put with hash key
	HASH *newhash = (HASH *)malloc(sizeof(HASH)); newhash->link = NULL;
	while(EOF != fscanf(fp, "%s\t%s\t%s\n", newhash->opcode, newhash->name, newhash->format)){
		if(newhash->name[0] == 'A' || newhash->name[0] == 'B')
			Inserthash(&hash[0], newhash);
		else if(newhash->name[0] == 'C' || newhash->name[0] == 'D')
			Inserthash(&hash[1], newhash);
		else if(newhash->name[0] == 'E' || newhash->name[0] == 'F')
			Inserthash(&hash[2], newhash);
		else if(newhash->name[0] == 'G' || newhash->name[0] == 'H')
			Inserthash(&hash[3], newhash);
		else if(newhash->name[0] == 'I' || newhash->name[0] == 'J')
			Inserthash(&hash[4], newhash);
		else if(newhash->name[0] == 'K' || newhash->name[0] == 'L')
			Inserthash(&hash[5], newhash);
		else if(newhash->name[0] == 'M' || newhash->name[0] == 'N')
			Inserthash(&hash[6], newhash);
		else if(newhash->name[0] == 'O' || newhash->name[0] == 'P')
			Inserthash(&hash[7], newhash);
		else if(newhash->name[0] == 'Q' || newhash->name[0] == 'R')
			Inserthash(&hash[8], newhash);
		else if(newhash->name[0] == 'S' || newhash->name[0] == 'T')
			Inserthash(&hash[9], newhash);
		else if(newhash->name[0] == 'U' || newhash->name[0] == 'V')
			Inserthash(&hash[10], newhash);
		else if(newhash->name[0] == 'W' || newhash->name[0] == 'X')
			Inserthash(&hash[11], newhash);
		else if(newhash->name[0] == 'Y' || newhash->name[0] == 'Z')
			Inserthash(&hash[12], newhash);
		else Inserthash(&hash[13], newhash);

	}

	fclose(fp);
}

/* Interhash */
// HASH 포인터에 새로운 HASH를 삽입한다.
// input: HASH **h, HASH *new, return: void

void Inserthash(HASH **h, HASH *new){
	HASH *curr = *h;

	if(*h == NULL) {
		*h = (HASH *)malloc(sizeof(HASH)); 
		(*h)->link = NULL;
		strcpy((*h)->opcode, new->opcode);
		strcpy((*h)->name, new->name);
		strcpy((*h)->format, new->format);
		return;
	}

	while(curr->link != NULL) {
		curr = curr->link;
	}
	HASH *newhash = (HASH *)malloc(sizeof(HASH));
	newhash->link = NULL; strcpy(newhash->opcode, new->opcode);
	strcpy(newhash->name, new->name); strcpy(newhash->format, new->format);
	curr->link = newhash;
}

/* Opcodemn */
// opcode mnemonic 명령어를 수행한다.
// input: char mnemonic[], return: 0 or 1

int Opcodemn(char mnemonic[]){
	int curridx = ((int)(mnemonic[0]) - 65) / 2; // idx(hash value)
	HASH *curr = hash[curridx];
	// find opcode w/ mnemonic
	while(curr != NULL){
		if(strcmp(curr->name, mnemonic) == 0) {
			printf("opcode is %s\n", curr->opcode); return 1;
		}
		else curr = curr->link;
	}
	if(curr == NULL) printf("There is no opcode of %s\n", mnemonic);
	return 1;
}

/* Opcodelist */
// opcodelist 명령어를 수행한다.
// input, return: void

void Opcodelist(){
	for(int i = 0;i < 20;i++){
		printf("%d : ", i);
		HASH *curr = hash[i];
		if(curr != NULL) {
			printf("[%s,%s]", curr->name, curr->opcode);
			curr = curr->link;
		}
		while(curr != NULL){
			printf(" -> [%s,%s]", curr->name, curr->opcode);
			curr = curr->link;
		}
		printf("\n");
	}
}

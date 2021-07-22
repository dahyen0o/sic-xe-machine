#include "20191657.h"

typedef struct SYMBOL{
	char symbol[10]; int add;
} symbol;
int symflag = 0;

/* Symbol */
// Assemble 함수에서 만들어진 symbol table 이 있으면 알맞게 출력한다.
// input: void, output: 0 or 1

int Symbol(){
	if(!symflag){printf("No symtab\n"); return 1;}

	int i, j, k;
	symbol sym[500];

	// copy
	for(i = 0, j = 0;i < symidx;i++){
		if(strlen(ssymtab[i].label) != 0 && ssymtab[i].label[0] != '.'){
			if(!strcmp(ssymtab[i].opcode, "START")) continue;
			strcpy(sym[j].symbol, ssymtab[i].label);
			sym[j].add = ssymtab[i].address;
			j++;
		}
	}

	// print
	for(i = j - 1;i > 0;i--){
		for(k = 0;k < i;k++){
			if(strcmp(sym[k].symbol, sym[k + 1].symbol) > 0){
				char tmp[10];
				strcpy(tmp, sym[k].symbol);
				strcpy(sym[k].symbol, sym[k + 1].symbol);
				strcpy(sym[k + 1].symbol, tmp);

				int t = sym[k].add;
				sym[k].add = sym[k + 1].add;
				sym[k + 1].add = t;
			}
		}
	}

	for(i = 0;i < j;i++) {
		printf("\t%-s\t%04X", sym[i].symbol, sym[i].add);
		if(i != j - 1) printf("\n");
	}
	printf("\n");
	return 1;
}

/* Assemble */
// 받은 파일이 .asm 파일이면 두 pass를 거쳐 알맞은 obj, lst 파일 및 symbol table을 생성한다.
// input: char filename[], output: 0 or 1

int Assemble(char filename[]){	
	FILE *finput = fopen(filename, "r");
	if(finput == NULL){printf("Error - line 0\n"); return 0;}

	/* Pass 1 */
	symidx = 0;
	int i, j, p1_start, p1_flag = 0;
	int text[500][3] = {0, }, texti = 0; // text[][i]-> 0: text record start add, 1: length, 2: code 개수
	int mod[500] = {0, }, modi = 0;
	long locctr = -1;
	char line[100], label[100], opcode[100], oprand[100];
	SYMTAB tmp_symtab[500]; // temp symtab
	
	// i: line
	for(i = 0;fgets(line, 100, finput) != NULL;i += 5){
		int k = 0;
		for(j = 0;j < strlen(line);j++){ // label
			if(line[j] == ' ' || line[j] == '\n') break;
			else label[k++] = line[j];
		}
		label[k] = '\0';
		if(strcmp(".", label) == 0){ // 주석
			for(k = 0;j < strlen(line);j++){
				if(line[j] == '\n') break;
				else opcode[k++] = line[j];
			}
			opcode[k] = '\0';
			oprand[0] = '\0';
		}
		else{
			for(;j < strlen(line);j++){
				if(line[j] != ' ') break;
			}
			for(k = 0;j < strlen(line);j++){
				if(line[j] == ' ') break;
				else opcode[k++] = line[j];
			}
			opcode[k] = '\0';
			if(opcode[k - 1] == '\n'){
				opcode[k - 1] = '\0'; oprand[0] = '\0';
			}
			else{
				for(;j < strlen(line);j++){
					if(line[j] != ' ') break;
				}
				for(k = 0;j < strlen(line);j++){
					if(line[j] == '\n') break;
					else oprand[k++] = line[j];
				}
				oprand[k] = '\0';
			}
		}

		//if(strlen(label) != 0) printf("label:%s\n", label);
		//if(strlen(opcode) != 0) printf("opcode: %s\n", opcode);
		//if(strlen(oprand) != 0) printf("oprand: %s\n", oprand);

		if(strlen(label) > 0 && *label == '.') { // 주석
			strcpy(tmp_symtab[symidx].label, label);
			strcpy(tmp_symtab[symidx].opcode, opcode);
			strcpy(tmp_symtab[symidx++].oprand, "");
			continue;
		}
		else if(strlen(label) > 0 && (*label < 'A' || *label > 'Z')){
			printf("Error - line %d\n", i); return 0;
		} // label의 첫글자는 영문
		
		if(locctr == -1 && strcmp(opcode, "START") == 0){ // START
			// operand 검사
			for(j = 0;j < strlen(oprand);j++){
				if(*(oprand + j) >= '0' && *(oprand + j) <= '9') continue;
				else if(*(oprand + j) >= 'A' && *(oprand + j) <= 'F') continue;
				else{printf("Error - line %d\n", i); return 0;}
			}

			// SYMTAB에 저장	
			strcpy(tmp_symtab[symidx].label, label);
			strcpy(tmp_symtab[symidx].opcode, opcode);
			strcpy(tmp_symtab[symidx].oprand, oprand);

			locctr = strtoul(tmp_symtab[symidx].oprand, NULL, 16);
			p1_start = locctr;
			tmp_symtab[symidx].address = locctr;

			// obj
			text[texti][0] = p1_start;
		}
		else if(locctr > -1){ // after START
			tmp_symtab[symidx].address = locctr;

			if(strlen(label) > 0){ // label
				for(j = 0;j < symidx;j++){
					if(strlen(tmp_symtab[j].label) == 0) continue;
					if(strcmp(label, tmp_symtab[j].label) == 0){
						printf("Error - line %d\n", i); return 0;
					} // find 중복label
				}
				strcpy(tmp_symtab[symidx].label, label);
			}
			else tmp_symtab[symidx].label == NULL;

			// opcode, oprand
			// opcode 저장
			strcpy(tmp_symtab[symidx].opcode, opcode);
			if(strcmp(tmp_symtab[symidx].opcode, "END") == 0){
				strcpy(tmp_symtab[symidx].oprand, oprand);
				symidx++; i += 5;
				locctr -= tmp_symtab[0].address;
				
				// tmp_symtab -> symtab 복사

				for(j = 0;j < symidx;j++){
					strcpy(symtab[j].label, tmp_symtab[j].label);
					strcpy(symtab[j].opcode, tmp_symtab[j].opcode);
					strcpy(symtab[j].oprand, tmp_symtab[j].oprand);
					strcpy(symtab[j].format, tmp_symtab[j].format);
					symtab[j].address = tmp_symtab[j].address;	
					symtab[j].object = tmp_symtab[j].object;	
				}

				p1_flag = 1; 
				break;
			}
			else if(strcmp(tmp_symtab[symidx].opcode, "BYTE") == 0){
				strcpy(tmp_symtab[symidx].oprand, oprand);
				if(tmp_symtab[symidx].oprand[0] == 'C'){
					if(text[texti][1] + 6 > 60) text[++texti][0] = locctr;
					text[texti][1] += 6; text[texti][2]++;// obj

					locctr += strlen(oprand) - 3;
				}
				else if(tmp_symtab[symidx].oprand[0] == 'X'){
					if(text[texti][1] + 2 > 60) text[++texti][0] = locctr;
					text[texti][1] += 2; text[texti][2]++;// obj
					for(j = 2;j < 4;j++){
						if(oprand[j] >= '0' && oprand[j] <= '9') continue;
						else if(oprand[j] >= 'A' && oprand[j] <= 'F') continue;
						else{
							printf("Error - line %d\n", i);
							return 0;
						}
					}
					locctr++;
				}
			}

				else if(strcmp(tmp_symtab[symidx].opcode, "RESB") == 0){
					if(text[texti][2] > 0) texti++; // obj

					strcpy(tmp_symtab[symidx].oprand, oprand);
					locctr += atoi(oprand);
				}
				else if(strcmp(tmp_symtab[symidx].opcode, "RESW") == 0){
					if(text[texti][2] > 0) texti++; // obj 

					strcpy(tmp_symtab[symidx].oprand, oprand);
					locctr += 3 * atoi(oprand);
				}
				else if(strcmp(tmp_symtab[symidx].opcode, "WORD") == 0){
					strcpy(tmp_symtab[symidx].oprand, oprand);
					locctr += 3;
				}
				else if(strcmp(tmp_symtab[symidx].opcode, "BASE") == 0){
					strcpy(tmp_symtab[symidx].oprand, oprand);
					tmp_symtab[symidx].address = 0;
				}
				else{ // other opcode
					// find opcode of OPTAB
					int flag = 0; char *tmp_opcode;
					if(opcode[0] == '+') {
						tmp_opcode = opcode + 1;
					}
					else tmp_opcode = opcode;

					HASH *curr = hash[((int)(*tmp_opcode) - 65) / 2];
					for(;curr != NULL;curr = curr->link){
						if(strcmp(tmp_opcode, curr->name) == 0){
							strcpy(tmp_symtab[symidx].oprand, oprand);
							tmp_symtab[symidx].object = strtoul(curr->opcode, NULL, 16); 
							strcpy(tmp_symtab[symidx].format, curr->format);

							// obj
							if(!strcmp("JSUB", curr->name)) {
								for(j = 0;j < texti;j++) mod[modi] += text[j][1] / 2;
								mod[modi++] += (text[texti][1] + 2) / 2;
							}
							
							if((curr->format)[0] == '2') {
								if(text[texti][1] + 4 > 60){ // next line
									text[++texti][0] = locctr; 
									text[texti][1] += 4;
								}
								else text[texti][1] += 4;
								text[texti][2]++;
							}
							else if(opcode[0] == '+'){
								if(text[texti][1] + 8 > 60) text[++texti][0] = locctr;
								text[texti][1] += 8; text[texti][2]++;
							}
							else if((curr->format)[0] == '3') {
								if(text[texti][1] + 6 > 60){
									text[++texti][0] = locctr; 
									text[texti][1] += 6;
								}
								else text[texti][1] += 6;
								text[texti][2]++;
							}
							else if((curr->format)[0] == '1') {
								if(text[texti][1] + 2 > 60){
									text[++texti][0] = locctr; 
									text[texti][1] += 2;
								}
								else text[texti][1] += 2;
								text[texti][2]++;
							}

							if(opcode[0] == '+') locctr += 4;
							else if(tmp_symtab[symidx].format[0] == '1') locctr += 1;
							else if(tmp_symtab[symidx].format[0] == '2') locctr += 2;
							else locctr += 3;
							flag = 1; 	
							break;
						}
					}
					if(flag == 0){
						printf("Error - line %d\n", i); return 0;
					}
				}
		}
		else continue; // not START yet
		
		symidx++;
	}

	if(p1_flag == 0){printf("Error - line %d\n", i); return 0;}
	
	/*for(i = 0;i < symidx;i++){ //////
		if(strlen(symtab[i].label) > 0) printf("Lable: %s ", symtab[i].label);
		if(strlen(symtab[i].opcode) > 0) printf("Opcode: %s ", symtab[i].opcode);
		if(strlen(symtab[i].oprand) > 0) printf("Oprand: %s ", symtab[i].oprand);
		if(strlen(symtab[i].format) > 0) printf("Format: %s ", symtab[i].format);
		printf("\n");
	}*/

	/*for(i = 0;i < symidx;i++){
		printf("%04X", symtab[i].address);
		if(strlen(symtab[i].label) > 0) printf("\t%s", symtab[i].label);
		printf("\n");
	}*/

	/*for(i = 0;i < texti + 1;i++){
		printf("%06X %02X %d\n", text[i][0], text[i][1], text[i][2]);
	}*/

	/* Pass 2 */
	// make name of obj and lst file
	char *objname = strtok(filename, "."), lstname[35];
	strcpy(lstname, objname);
	strcat(lstname, ".lst"); strcat(objname, ".obj");

	FILE *fobj = fopen(objname, "w"), *flst = fopen(lstname, "w");
	int p2_start = -1, p2_locctr, k;
	int base = -1, ti = 0, tcount = 0;

	for(i = 5, j = 0;j < symidx;i += 5, j++){
		// i: line, j: symidx
		if(p2_start == -1){ // not START yet
			if(strcmp("START", symtab[j].opcode) == 0){ // START
				// write at .lst
				fprintf(flst, "%-4d\t%04X\t", i, symtab[j].address);
				fprintf(flst, "%-8s\t", symtab[j].label);
				fprintf(flst, "%-8s\t%X\n", symtab[j].opcode, p1_start);

				p2_locctr = p2_start = p1_start;

				// write at .obj
				fprintf(fobj, "H%s  ", symtab[j].label);
				fprintf(fobj, "%06X%06X\nT%06X%02X", p1_start, (int)locctr, p1_start, text[ti][1] / 2);
			}
		}
		else if(p2_start > -1){ // already START
			if(strcmp(".", symtab[j].label) == 0){ // 주석
				fprintf(flst, "%-4d\t    \t.       \t", i);
				fprintf(flst, "%-8s\n", symtab[j].opcode);
				continue;
			}

			p2_locctr = symtab[j].address;

			if(text[ti][2] == tcount && strcmp("RESB", symtab[j].opcode) && strcmp("RESW", symtab[j].opcode)){ 
				// if line is full
				if(ti != texti){
					ti++; fprintf(fobj, "\nT%06X%02X", p2_locctr, text[ti][1] / 2);
					tcount = 0;
				}
			}

			if(strcmp("END", symtab[j].opcode) == 0){
				fprintf(flst, "%-4d\t    \t        \tEND     \t%-15s", i, symtab[j].oprand); // lst

				fprintf(fobj, "\n");
				// obj Mod, End
				for(k = 0; k < modi;k++) fprintf(fobj, "M%06X05\n", mod[k]);
				fprintf(fobj, "E%06X", p1_start);

				// copy symtab -> ssymtab
				for(j = 0;j < symidx;j++){
					strcpy(ssymtab[j].label, symtab[j].label);
					strcpy(ssymtab[j].opcode, symtab[j].opcode);
					strcpy(ssymtab[j].oprand, symtab[j].oprand);
					strcpy(ssymtab[j].format, symtab[j].format);
					ssymtab[j].address = symtab[j].address;	
					ssymtab[j].object = symtab[j].object;	
				}
				symflag = 1;

				fclose(fobj); fclose(flst);
				return 1;
			}
			else if(strcmp("BYTE", symtab[j].opcode) == 0){
				if(symtab[j].oprand[0] == 'X'){
					char b[3]; 
					b[0] = symtab[j].oprand[2];
					b[1] = symtab[j].oprand[3];
					b[2] = '\0';

					// write at .lst
					fprintf(flst, "%-4d\t%04X\t", i, symtab[j].address);
					fprintf(flst, "%-8s\tBYTE    \t", symtab[j].label);
					fprintf(flst, "%-15s\t%s\n", symtab[j].oprand, b);

					// write at .obj
					fprintf(fobj, "%s", b); tcount++;
				}
				else if(symtab[j].oprand[0] == 'C'){
					// write at .lst
					fprintf(flst, "%-4d\t%04X\t", i, symtab[j].address);
					fprintf(flst, "%-8s\tBYTE    \t", symtab[j].label);
					fprintf(flst, "%-15s\t", symtab[j].oprand);

					// obj
					for(k = 2;k < strlen(symtab[j].oprand) - 1;k++){
						fprintf(flst, "%02X", (int)(symtab[j].oprand[k]));
						fprintf(fobj, "%02X", (int)(symtab[j].oprand[k]));
						tcount++;
					}
					fprintf(flst, "\n");
				}
			}
			else if(strcmp("WORD", symtab[j].opcode) == 0){
				// write at .lst
				fprintf(flst, "%-4d\t%04X\t", i, symtab[j].address);
				fprintf(flst, "%-8s\tWORD    \t", symtab[j].label);
				fprintf(flst, "%-15s\t", symtab[j].oprand);
				fprintf(flst, "%06X\n", atoi(symtab[j].oprand));

				// write at .obj
				fprintf(fobj, "%06X\n", atoi(symtab[j].oprand));
				tcount++;
				
			}
			else if(strcmp("BASE", symtab[j].opcode) == 0){
				for(k = j + 1;k < symidx;k++){
					if(!strcmp(symtab[j].oprand, symtab[k].label)){
						base = symtab[k].address;
						break;
					}
				}
				if(k == symidx){ // not found
					printf("Error - line %d\n", i);
					fclose(fobj); fclose(flst); 
					remove(lstname); remove(objname);
					return 0;
				}
				
				// write at .lst
				fprintf(flst, "%-4d\t    \t        \t", i);
				fprintf(flst, "BASE    \t%-15s\n", symtab[j].oprand);
			}
			else if(strcmp("RESB", symtab[j].opcode) == 0){
				// write at .lst
				fprintf(flst, "%-4d\t%04X\t", i, symtab[j].address);
				fprintf(flst, "%-8s\tRESB    \t", symtab[j].label);
			    fprintf(flst, "%-15s\n", symtab[j].oprand);

				tcount = text[ti][2];
			}
			else if(strcmp("RESW", symtab[j].opcode) == 0){
				// write at .lst
				fprintf(flst, "%-4d\t%04X\t", i, symtab[j].address);
				fprintf(flst, "%-8s\tRESW    \t", symtab[j].label);
				fprintf(flst, "%-15s\n", symtab[j].oprand);
				
				tcount = text[ti][2];
			}
			else{ // search OBTAB
				fprintf(flst, "%-4d\t%04X\t", i, symtab[j].address);
				if(symtab[j].label != NULL)
					fprintf(flst, "%-8s\t", symtab[j].label);
				else fprintf(flst, "        \t");
				fprintf(flst, "%-8s\t", symtab[j].opcode);
				fprintf(flst, "%-15s\t", symtab[j].oprand);
				
				if(symtab[j].opcode[0] == '+'){ // format 4 - dir add
					if(symtab[j].oprand[0] == '#'){ // imm add
						// n = 0, i = 1
						char *constant = symtab[j].oprand + 1;
						// write at .lst
						fprintf(flst, "%02X1", symtab[j].object + 1);
						// obj
						fprintf(fobj, "%02X1", symtab[j].object + 1);

						if(symtab[j].oprand[1] >= '0' && symtab[j].oprand[1] <= '9'){ // oprand is integer
							fprintf(flst, "%05X\n", atoi(constant));
							fprintf(fobj, "%05X", atoi(constant));
							tcount++;
						}
						else{ // oprand is var(label)
							for(k = 0;k < symidx;k++){ // find label
								if(!strcmp(symtab[k].label, symtab[j].oprand + 1)){
									fprintf(flst, "%05X\n", symtab[k].address);
									fprintf(fobj, "%05X", symtab[k].address);
									tcount++; break;
								}
							}
							if(k == symidx){
								fclose(fobj); fclose(flst); 
								remove(lstname); remove(objname);
								printf("Error - line %d\n", i); return 0;
							}
						}
					}
					else{ // sim add
						// n = 1, i = 1
						fprintf(flst, "%02X1", symtab[j].object + 3);
						fprintf(fobj, "%02X1", symtab[j].object + 3);
						
						for(k = 0;k < symidx;k++){ // find label
							if(!strcmp(symtab[k].label, symtab[j].oprand)){
								fprintf(flst, "%05X\n", symtab[k].address);
								fprintf(fobj, "%05X", symtab[k].address);
								tcount++;
								break;
							}
						}
						if(k == symidx){
							fclose(fobj); fclose(flst); 
							remove(lstname); remove(objname);
							printf("Error - line %d\n", i); return 0;
						}
					}
				}
				else{ // format 1-3
					if(symtab[j].format[0] == '1'){ // format 1
						fprintf(flst, "%02X\n", symtab[j].object);
						fprintf(fobj, "%02X", symtab[j].object);
						tcount++;
					}
					else if(symtab[j].format[0] == '2'){ // format 2
						fprintf(flst, "%02X", symtab[j].object);
						fprintf(fobj, "%02X", symtab[j].object);
						tcount++;

						for(k = 0;k < strlen(symtab[j].oprand) - 1;k++){
							if(symtab[j].oprand[k] == 'A'){
								fprintf(flst, "0"); fprintf(fobj, "0");
							}
							else if(symtab[j].oprand[k] == 'X'){
								fprintf(flst, "1"); fprintf(fobj, "1");
							}
							else if(symtab[j].oprand[k] == 'L'){
								fprintf(flst, "2"); fprintf(fobj, "2");
							}
							else if(symtab[j].oprand[k] == 'P'){
								if(symtab[j].oprand[k + 1] == 'C'){
									fprintf(flst, "8");
									fprintf(fobj, "8");
								}
								else{
									fclose(fobj); fclose(flst); 
									remove(lstname); remove(objname);
									printf("Error - line %d\n", i);
									return 0;
								}
							}
							else if(symtab[j].oprand[k] == 'S'){
								if(symtab[j].oprand[k + 1] == 'W'){
									fprintf(flst, "9");
									fprintf(fobj, "9");
								}
								else {
									fprintf(flst, "4"); fprintf(fobj, "4");
								}
							}
							else if(symtab[j].oprand[k] == 'B'){
								fprintf(flst, "3"); fprintf(fobj, "3");
							}
							else if(symtab[j].oprand[k] == 'T'){
								fprintf(flst, "5"); fprintf(fobj, "5");
							}
							else if(symtab[j].oprand[k] == 'F'){
								fprintf(flst, "6"); fprintf(fobj, "6");
							}
						}
						if(symtab[j].oprand[k] == 'A'){
							fprintf(flst, "0"); fprintf(fobj, "0");
						}
						else if(symtab[j].oprand[k] == 'X'){
							fprintf(flst, "1"); fprintf(fobj, "1");
						}
						else if(symtab[j].oprand[k] == 'L'){
							fprintf(fobj, "2");
							fprintf(flst, "2");
						}
						else if(symtab[j].oprand[k] == 'S'){
							fprintf(flst, "4"); fprintf(fobj, "4");
						}
						else if(symtab[j].oprand[k] == 'B'){
							fprintf(flst, "3"); fprintf(fobj, "3");
						}
						else if(symtab[j].oprand[k] == 'T'){
							fprintf(flst, "5"); fprintf(fobj, "5");
						}
						else if(symtab[j].oprand[k] == 'F'){
							fprintf(flst, "6"); fprintf(fobj, "6");
						}

						for(k = 0;k < strlen(symtab[j].oprand);k++){
							if(symtab[j].oprand[k] == ',') break;
						}
						if(k == strlen(symtab[j].oprand)) {
							fprintf(flst, "0"); fprintf(fobj, "0");
						}
						fprintf(flst, "\n");
					}
					else if(!strcmp(symtab[j].opcode, "RSUB")){ // RSUB
						fprintf(flst, "4F0000\n");
						fprintf(fobj, "4F0000"); 
						tcount++;
					}
					else if(!strcmp(symtab[j].opcode, "STCH") || !strcmp(symtab[j].opcode, "LDCH")){ // STCH, LDCH
						int pc, nextadd;
						if(!strcmp(symtab[j].opcode, "STCH")){
							fprintf(flst, "57"); fprintf(fobj, "57");
						}
						else{
							fprintf(flst, "53"); fprintf(fobj, "53");
						}
						
						char oprand[40];
						for(k = 0;k < strlen(symtab[j].oprand);k++){
							if(symtab[j].oprand[k] == ',') {
								oprand[k] = '\0'; break;
							}
							oprand[k] = symtab[j].oprand[k];
						}

						for(k = 0;k < symidx;k++){
							if(!strcmp(symtab[k].label, oprand)){
								pc = symtab[k].address;
								break;
							}	
						}
						if(k == symidx){
							fclose(fobj); fclose(flst); 
							remove(lstname); remove(objname);
							printf("Error - line %d\n", i); return 0;
						}

						nextadd = symtab[j + 1].address;
						for(k = j + 1;symtab[k].address < 1;k++)
						   	nextadd = symtab[k].address; // find next add
						if(pc - nextadd < -2048 || pc - nextadd > 2047){ // BASE
							int x;
							if(pc - base < 0) x = 4096 + pc - base;
							else x = pc - base;

							fprintf(flst, "C%03X\n", x);
							fprintf(fobj, "C%03X", x);
						}
						else{ // PC
							int x;
							if(pc - nextadd < 0) x = 4096 + pc - nextadd;
							else x = pc - nextadd;
							fprintf(flst, "A%03X\n", x);
							fprintf(fobj, "A%03X", x);
						}
						tcount++;
					}
					else{ // format 3 except RSUB, STCH, LDCH
						// try PC
						int pc, nextadd;
						if(symtab[j].oprand[0] == '@'){ // indir add
							for(k = 0;k < symidx;k++){
								if(!strcmp(symtab[k].label, symtab[j].oprand + 1)){
									pc = symtab[k].address;
									break;
								}	
							}
							if(k == symidx){
								fclose(fobj); fclose(flst); 
								remove(lstname); remove(objname);
								printf("Error - line %d\n", i); return 0;
							}
						
							fprintf(flst, "%02X", symtab[j].object + 2);
							fprintf(fobj, "%02X", symtab[j].object + 2);

							nextadd = symtab[j + 1].address;
							for(k = j + 1;symtab[k].address < 1;k++)
							   	nextadd = symtab[k].address; // find next add
							if(pc - nextadd < -2048 || pc - nextadd > 2047){ // BASE
								int x;
								if(pc - base < 0) x = 4096 + pc - base;
								else x = pc - base;

								fprintf(flst, "4%03X\n", x);
								fprintf(fobj, "4%03X", x);
							}
							else{ // PC
								int x;
								if(pc - nextadd < 0) x = 4096 + pc - nextadd;
								else x = pc - nextadd;
								fprintf(flst, "2%03X\n", x);
								fprintf(fobj, "2%03X", x);
							}
							tcount++;
						}
						else if(symtab[j].oprand[0] == '#'){ // imm add
							fprintf(flst, "%02X", symtab[j].object + 1);
							fprintf(fobj, "%02X", symtab[j].object + 1);
							tcount++;

							if(symtab[j].oprand[1] >= '0' && symtab[j].oprand[1] <= '9'){ // number
								fprintf(flst, "0%03X\n", atoi(symtab[j].oprand + 1));
								fprintf(fobj, "0%03X", atoi(symtab[j].oprand + 1));
							}
							else{ // variable(char [])
								for(k = 0;k < symidx;k++){ // find add
									if(!strcmp(symtab[k].label, symtab[j].oprand + 1)){
										pc = symtab[k].address; break;
									}
								}
								if(k == symidx){
									fclose(fobj); fclose(flst); 
									remove(lstname); remove(objname);
									printf("Error - line %d\n", i); return 0;
								}

								for(k = j + 1;symtab[k].address == 0;k++);
								nextadd = symtab[k].address;

								if(pc - nextadd < -2048 || pc - nextadd > 2047){ // BASE
									int x;
									if(pc - base < 0) x = 4096 + pc - base;
									else x = pc - base;

									fprintf(flst, "4%03X\n", x); 
									fprintf(fobj, "4%03X", x); 
								}
								else{ // PC
									int x;
									if(pc - nextadd < 0) x = 4096 + pc - nextadd;
									else x = pc - nextadd;

									fprintf(flst, "2%03X\n", x);
									fprintf(fobj, "2%03X", x);
								}
							}
						}
						else{ // simple add
							fprintf(flst, "%02X", symtab[j].object + 3);
							fprintf(fobj, "%02X", symtab[j].object + 3);
							tcount++;

							for(k = 0;k < symidx;k++){ // find add
								if(!strcmp(symtab[k].label, symtab[j].oprand)){
									pc = symtab[k].address; break;
								}
							}
							if(k == symidx){
								fclose(fobj); fclose(flst); 
								remove(lstname); remove(objname);
								printf("Error - line %d\n", i); return 0;
							}
							
							nextadd = symtab[j + 1].address;
							for(k = j + 1;symtab[k].address < 1;k++)
								nextadd = symtab[k].address; // find next add
							if(pc - nextadd < -2048 || pc - nextadd > 2047){ // BASE
								int x;
								if(pc - base < 0) x = 4096 + pc - base;
								else x = pc - base;

								fprintf(flst, "4%03X\n", x); 
								fprintf(fobj, "4%03X", x); 
							}
							else{ // PC
								int x;
								if(pc - nextadd < 0) x = 4096 + pc - nextadd;
								else x = pc - nextadd;
								
								fprintf(flst, "2%03X\n", x);
								fprintf(fobj, "2%03X", x);
							}
						}
					}
				}
			}
		}
		else continue;
	}

	printf("Error - line %d\n", i);
	fclose(fobj); fclose(flst); 
	remove(lstname); remove(objname);
	return 0;
}

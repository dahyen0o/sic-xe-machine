#include "20191657.h"
int proglen;

int Progadd(char address[]){
	progaddr = (int)strtoul(address, NULL, 16);
	return 1;
}

void Loaderprint(){
	proglen = 0;
	printf("control symbol address length\n");
	printf("section name\n");
	printf("-------------------------------\n");
	for(int i = 0;i < esidx;i++){
		if(strlen(estab[i].cs)) {// cs
			printf("%7s          %04X   %04X\n", 
					estab[i].cs, estab[i].address, estab[i].length);
			proglen += estab[i].length;
		}
		else
			printf("        %-6s   %04X\n",
					estab[i].name, estab[i].address);
	}
	printf("-------------------------------\n");
	printf("          total length %04X\n", proglen);
}

int Loader1(char filename[], int pidx){
	/* Pass 1 */
	FILE *finput = fopen(filename, "r");
	if(finput == NULL) {printf("Error - finput\n"); return 0;}

	if(pidx == 0) {
		execaddr = esidx = cslth = 0; 
		csaddr = progaddr;
	}
	char line[100], *lptr = NULL;
	while(fgets(line, 99, finput)){
		//printf("line: %s\n", line); //
		if(*line == 'H'){ // H record
			lptr = strtok(line, " ");

			for(int i = 0;i < esidx;i++){
				if(strlen(estab[i].cs) > 0 && !strcmp(lptr + 1, estab[i].cs)) {
					printf("Error - c.s name\n"); return 0;
				}
			}
			strcpy(estab[esidx].cs, lptr + 1);
			strcpy(estab[esidx].name, lptr + 1);

			lptr = strtok(NULL, " ");
			estab[esidx].length = (int)strtoul(lptr + 6, NULL, 16);

			csaddr += cslth;
			estab[esidx].address = csaddr;
			cslth = estab[esidx].length;
			estab[esidx].name[0] = '\0';
			esidx++;
		}
		else if(*line == 'D'){ // D record
			char syname[7], syadd[7]; // current symbol name
			int i, j;
			for(i = 1;i < strlen(line) - 1;i += 12){ // idx of line
				// get syname out of line
				// i: start idx of symbol name
				for(j = 0;j < 6;j++){
					if(line[j + i] != ' ') syname[j] = line[j + i];
					else break;
				}
				syname[j] = '\0';
				//printf("symname %s\n", syname);

				// check 중복 symbol name
				for(j = 0;j < esidx;j++){
					if(strlen(estab[j].name) > 0 && !strcmp(estab[j].name, syname)){
						printf("Error - sym name\n"); return 0;
					}
				}

				estab[esidx].cs[0] = '\0';
				strcpy(estab[esidx].name, syname);

				// get address out of line
				for(j = 0;j < 6;j++){
					syadd[j] = line[i + 6 + j];
				}
				estab[esidx].address = (int)strtoul(syadd, NULL, 16) + csaddr;
				estab[esidx].length = 0;
				esidx++;
			}
		}
	}
	fclose(finput);
	return 1;
}

int Loader2(char filename[], int pidx){
	/* Pass 2 */
	FILE *finput = fopen(filename, "r");
	if(finput == NULL) {printf("Error - finput\n"); return 0;}
	if(pidx == 0) {
		cslth = 0; csaddr = progaddr;
	}
	char refname[20][7]; char line[100];

	while(fgets(line, 99, finput)){
		line[strlen(line) - 1] = '\0'; // 엔터 제거
		if(line[0] == 'T'){ // T record	
			char stadd[7], text[70];
			strncpy(stadd, line + 1, 6);
			stadd[6] = '\0';
			strcpy(text, line + 9);
			int intadd = (int)strtoul(stadd, NULL, 16) + csaddr;
			//printf("csaddr:%X\n", csaddr);

			for(int i = 0;i < strlen(text);i += 2){
				char add[7], value[3];
				sprintf(add, "%X", intadd++);
				strncpy(value, text + i, 2);
				value[2] = '\0';
				//printf("add:%s value:%s\n", add, value);
				Edit(add, value);
			}
		}
		else if(line[0] == 'M'){ // M record
			int refnum = (int)strtoul(line + 10, NULL, 16);
			for(int i = 0;i < esidx;i++){
				if(refnum == 1 || (strlen(estab[i].name) > 0 && !strcmp(estab[i].name, refname[refnum]))){ // modify
					char stadd[7], currmem[13]; 
					int intstadd, intcurrmem;
					strncpy(stadd, line + 1, 6);
					stadd[6] = '\0';
					intstadd = (int)strtoul(stadd, NULL, 16) + csaddr;

					int sti, stj, k; 
					// sti: mem[i], stj: memory[0][j]
					sti = intstadd / 16;
					stj = intstadd % 16; stj *= 2;
					if((line[8] - '0') % 2) stj += 1;
					for(k = 0;k < (line[8] - '0');stj++, k++){ 
						// memory[j] (bit idx), k: currmem idx
						if(stj >= 32) {
							sti++; stj = 0;
						}
						currmem[k] = mem[sti].memory[stj / 2][stj % 2];
						//printf("%c = mem[%X].mem[%d][%d]\n", currmem[k], sti, stj / 2, stj % 2);
					}
					currmem[k] = '\0';
					//printf("currmem:%s\n", currmem);

					intcurrmem = (int)strtoul(currmem, NULL, 16);
					if(refnum != 1){
						if(line[9] == '+') intcurrmem += (estab[i].address);
						else intcurrmem -= (estab[i].address);
					}
					else{
						if(line[9] == '+') intcurrmem += csaddr;
						else intcurrmem -= csaddr;
					}
					
					char tmpmem[9];
					sprintf(tmpmem, "%08X", intcurrmem); // 8 자리
					if((line[8] - '0') % 2) {
						strcpy(currmem, tmpmem + 3);
					}
					else {
						strcpy(currmem, tmpmem + 2);
					}
					//printf("currmem %s\n", currmem);

					sti = intstadd / 16; 
					stj = intstadd % 16;
					char ch[3];
					if((line[8] - '0') % 2){
						sprintf(stadd, "%0X", intstadd);
						ch[0] = mem[sti].memory[stj][0];
						//printf("ch[0]:%X,%d\n", sti, stj);
						ch[1] = currmem[0];
						ch[2] = '\0';
						//printf("stadd:%s,ch:%s\n", stadd, ch);
						Edit(stadd, ch);
						k = 1; intstadd++;
					}
					else k = 0;
					for(;k < 5;intstadd++, k += 2) {
						sprintf(stadd, "%0X", intstadd);
						strncpy(ch, currmem + k, 2);
						ch[2] = '\0';
						//printf("stadd:%s,ch:%s\n", stadd, ch);
						Edit(stadd, ch);
					}
					break;
				}
			}
		}
		else if(line[0] == 'E'){ // E record
			if(line[1] != '\0') 
				execaddr = csaddr + (int)strtoul(line + 1, NULL, 16);
		}
		else if(line[0] == 'H'){
			char lptr[7]; strncpy(lptr, line + 13, 6);
			lptr[6] = '\0';
			csaddr += cslth;
			cslth = (int)strtoul(lptr, NULL, 16);
		}
		else if(*line == 'R'){ // R record
			for(int i = 1;i < strlen(line);i += 8){
				char num[3], name[7];
				strncpy(num, line + i, 2);
				num[2] = '\0';
				int j, refnum = (int)strtoul(num, NULL, 16);
				for(j = 0;j < 6;j++) {
					if(line[i + 2 + j] != ' ' && line[i + 2 + j] != '\0') 
						name[j] = line[i + j + 2];
					else break;
				}
				name[j] = '\0';
				strcpy(refname[refnum], name);
			}
		}
	}
	fclose(finput);
	return 1;
}

void Bp(char address[]){
	bparr[bpidx][1] = (int)strtoul(address, NULL, 16);
	bparr[bpidx][0] = 0;
	printf("\t\t[ok] create breakpoint %X\n", bparr[bpidx++][1]);
}

void Bpclear(){
	bpidx = 0;
	printf("\t\t[ok] clear all breakpoints\n");
}

void Bpprint(){
	printf("\t\tbreakpoint\n");
	printf("\t\t----------\n");
	for(int i = 0;i < bpidx;i++){
		printf("\t\t%X\n", bparr[i][1]);
	}
}

int Run(){
	int A, X, L, PC, B, S, T; // reg
	A = X = B = S = T = 0; PC = progaddr; L = proglen;

	int i, c = 0;
	// Pass 1: check except opcode using p1[]
	for(i = progaddr;i < progaddr + proglen;){
		// check format
		p1[i].mn = -1;
		//printf("curr i [%X]\n", i);
		// 1. get mnemonic with n, i
		char mnni[3]; int ni, fm, tmp = 0;
		mnni[0] = mem[i / 16].memory[i % 16][0];
		mnni[1] = mem[i / 16].memory[i % 16][1]; mnni[2] = '\0';
		int i_mnni = (int)strtoul(mnni, NULL, 16);

		// 2. find real mnemonic and n, i
		fm = ni = -1;
		if(!strcmp(mnni, "45")){ // BYTE 4544f46
			p1[i].object = (int)strtoul("454F46", NULL, 16);
			p1[i].format = 5; // byte = 5
			i += 3; continue;
		}
		else if(!strcmp(mnni, "F1")){ // BYTE
			p1[i].object = (int)strtoul("F1", NULL, 16);
			p1[i].format = 5;
			i += 1; continue;
		}
		else if(!strcmp(mnni, "05")){
			p1[i].object = (int)strtoul("05", NULL, 16);
			p1[i].format = 5;
			i += 1; continue;
		}

		for(int j = 0;j < 20;j++){
			for(HASH *curr = hash[j];curr != NULL;curr = curr->link){
				//printf("i[%X] j[%X] curr->name[%s]\n", i, j, curr->name);
				int i_currmn = (int)strtoul(curr->opcode, NULL, 16);
				switch(curr->format[0]){
					case '1': // format
						if(i_currmn == i_mnni){ // ni = 0
							tmp = 1;
							p1[i].object = (int)strtoul(mnni, NULL, 16);
							p1[i].format = 1;
							p1[i].mn = i_currmn;
							i += 1;
						}
						break;
					case '2':
						if(i_currmn == i_mnni){ // ni = 0
							//printf("######\n");
							tmp = 1;
							char temp[5]; strcpy(temp, mnni);
							strcat(temp, mem[(i + 1)/16].memory[(i + 1)%16]);
							p1[i].object = (int)strtoul(temp, NULL, 16);
							p1[i].format = 2;
							p1[i].mn = i_currmn;
							i += 2;
						}
						break;
					case '3': // 3 or 4
						if(i_currmn + 1 <= i_mnni && i_currmn + 3 >= i_mnni){
							if(mem[(i + 1)/16].memory[(i + 1)%16][0] == '1'){ // format 4
								char temp[9]; strcpy(temp, mnni);
								strcat(temp, mem[(i + 1)/16].memory[(i + 1)%16]);
								strcat(temp, mem[(i + 2)/16].memory[(i + 2)%16]);
								strcat(temp, mem[(i + 3)/16].memory[(i + 3)%16]);
								p1[i].object = (int)strtoul(temp, NULL, 16);
								p1[i].format = 4; p1[i].e = 1; // xbpe = 0001
								p1[i].mn = i_currmn;
							
								if(i_mnni == i_currmn + 1){
									p1[i].n = 0; p1[i].i = 1;
								}
								else if(i_mnni == i_currmn + 2){
									p1[i].n = 1; p1[i].i = 0;
								}
								else{
									p1[i].n = 1; p1[i].i = 1;
								}

								i += 4; tmp = 1;
							}
							// foramt 3
							else if(mem[(i + 1)/16].memory[(i + 1)%16][0] == '0'){
								// xbpe = 0000
								p1[i].x = p1[i].b = p1[i].e = 0; p1[i].p = 0;
								char temp[7]; strcpy(temp, mnni);
								strcat(temp, mem[(i + 1)/16].memory[(i + 1)%16]);
								strcat(temp, mem[(i + 2)/16].memory[(i + 2)%16]);
								p1[i].object = (int)strtoul(temp, NULL, 16);
								p1[i].format = 3;
								p1[i].mn = i_currmn;

								if(i_mnni == i_currmn + 1){
									p1[i].n = 0; p1[i].i = 1;
								}
								else if(i_mnni == i_currmn + 2){
									p1[i].n = 1; p1[i].i = 0;
								}
								else{
									p1[i].n = 1; p1[i].i = 1;
								}

								i += 3; tmp = 1;
							}
							else if(mem[(i + 1)/16].memory[(i + 1)%16][0] == '2'){
								// xbpe = 0010
								p1[i].x = p1[i].b = p1[i].e = 0; p1[i].p = 1;
								char temp[7]; strcpy(temp, mnni);
								strcat(temp, mem[(i + 1)/16].memory[(i + 1)%16]);
								strcat(temp, mem[(i + 2)/16].memory[(i + 2)%16]);
								p1[i].object = (int)strtoul(temp, NULL, 16);
								p1[i].format = 3;
								p1[i].mn = i_currmn;

								if(i_mnni == i_currmn + 1){
									p1[i].n = 0; p1[i].i = 1;
								}
								else if(i_mnni == i_currmn + 2){
									p1[i].n = 1; p1[i].i = 0;
	
								}
								else{
									p1[i].n = 1; p1[i].i = 1;
								}

								i += 3; tmp = 1;
							}
							else if(mem[(i + 1)/16].memory[(i + 1)%16][0] == '4'){
								// xbpe = 0100
								p1[i].x = p1[i].p = p1[i].e = 0; p1[i].b = 1;
								char temp[7]; strcpy(temp, mnni);
								strcat(temp, mem[(i + 1)/16].memory[(i + 1)%16]);
								strcat(temp, mem[(i + 2)/16].memory[(i + 2)%16]);
								p1[i].object = (int)strtoul(temp, NULL, 16);
								p1[i].format = 3;
								p1[i].mn = i_currmn;

								if(i_mnni == i_currmn + 1){
									p1[i].n = 0; p1[i].i = 1;
								}
								else if(i_mnni == i_currmn + 2){
									p1[i].n = 1; p1[i].i = 0;
								}
								else{
									p1[i].n = 1; p1[i].i = 1;
								}

								i += 3; tmp = 1;
							}
							else if(mem[(i + 1)/16].memory[(i + 1)%16][0] == 'A'){
								// xbpe = 1010
								p1[i].x = p1[i].p = 1; p1[i].e = p1[i].b = 0;
								char temp[7]; strcpy(temp, mnni);
								strcat(temp, mem[(i + 1)/16].memory[(i + 1)%16]);
								strcat(temp, mem[(i + 2)/16].memory[(i + 2)%16]);
								p1[i].object = (int)strtoul(temp, NULL, 16);
								p1[i].format = 3;
								p1[i].mn = i_currmn;

								if(i_mnni == i_currmn + 1){
									p1[i].n = 0; p1[i].i = 1;
								}
								else if(i_mnni == i_currmn + 2){
									p1[i].n = 1; p1[i].i = 0;
								}
								else{
									p1[i].n = 1; p1[i].i = 1;
								}

								i += 3; tmp = 1;
							}
							else if(mem[(i + 1)/16].memory[(i + 1)%16][0] == 'C'){
								// xbpe = 1100
								p1[i].x = p1[i].b = 1; p1[i].e = p1[i].p = 0;
								char temp[7]; strcpy(temp, mnni);
								strcat(temp, mem[(i + 1)/16].memory[(i + 1)%16]);
								strcat(temp, mem[(i + 2)/16].memory[(i + 2)%16]);
								p1[i].object = (int)strtoul(temp, NULL, 16);
								p1[i].format = 3;
								p1[i].mn = i_currmn;

								if(i_mnni == i_currmn + 1){
									p1[i].n = 0; p1[i].i = 1;
								}
								else if(i_mnni == i_currmn + 2){
									p1[i].n = 1; p1[i].i = 0;
								}
								else{
									p1[i].n = 1; p1[i].i = 1;
								}

								i += 3; tmp = 1;
							}
						}
						break;
				}
				if(tmp) break;
			}
			if(tmp) break;
		}
		if(!tmp) i++;
		if(i > 45 && i < 4150) i = 4150;
		//printf("&&i:%X\n", i);
	}

	/* Pass 2 */
	i = progaddr; length = 0;
	table[i].A = 0; table[i].X = 0;
	table[i].L = proglen; table[i].PC = i;
	table[i].B = 0; table[i].S = 0;
	table[i].T = 0;

	//printf("***\n");
}

int Run2(){
	int nexti;
	while(i < progaddr + proglen){
		//if(i == 0x426) scanf("%d", &nexti);
		//printf("curri:%X\n", i);
		//printf("A : %06X   X : %06X\n", table[i].A, table[i].X);
		//printf("L : %06X  PC : %06X\n", table[i].L, table[i].PC);
		//printf("B : %06X   S : %06X\n", table[i].B, table[i].S);
		//printf("T : %06X   len: %X\n", table[i].T, length);
		switch(p1[i].mn){
			case -1: // BYTE
				if(p1[i].object >= 16 * 16)
					nexti = i + 0x1009;
				else nexti = i + 1;
				table[nexti].A = table[i].A;
				table[nexti].X = table[i].X;
				table[nexti].L = table[i].L;
				table[nexti].PC = nexti;
				table[nexti].B = table[i].B;
				table[nexti].S = table[i].S;
				table[nexti].T = table[i].T;

				i = nexti;
				break;
			case 0: // LDA
				nexti = i + p1[i].format;
				table[nexti].X = table[i].X;
				table[nexti].L = table[i].L;
				table[nexti].PC = nexti;
				table[nexti].B = table[i].B;
				table[nexti].S = table[i].S;
				table[nexti].T = table[i].T;

				if(i / 16 == 0) table[nexti].A = length;
				else if(i % 16 == 10) table[nexti].A = (int)strtoul("454F46", NULL, 16);
				else table[nexti].A = 3;
				i = nexti;
				break;
			case 12: // STA
				nexti = i + p1[i].format;
				table[nexti].A = table[i].A;
				table[nexti].X = table[i].X;
				table[nexti].L = table[i].L;
				table[nexti].PC = nexti;
				table[nexti].B = table[i].B;
				table[nexti].S = table[i].S;
				table[nexti].T = table[i].T;
				if(i % 16 == 3) length = table[i].A;

				i = nexti;
				break;
			case 16: // STX
				nexti = i + p1[i].format;
				table[nexti].A = table[i].A;
				table[nexti].X = table[i].X;
				table[nexti].L = table[i].L;
				table[nexti].PC = nexti;
				table[nexti].B = table[i].B;
				table[nexti].S = table[i].S;
				table[nexti].T = table[i].T;
				length = table[i].X;

				i = nexti;
				break;
			case 20: // STL
				nexti = i + p1[i].format;
				table[nexti].A = table[i].A;
				table[nexti].X = table[i].X;
				table[nexti].L = table[i].L;
				table[nexti].PC = nexti;
				table[nexti].B = table[i].B;
				table[nexti].S = table[i].S;
				table[nexti].T = table[i].T;

				i = nexti;
				break;
			case 40: // COMP
				nexti = i + p1[i].format;
				table[nexti].A = table[i].A;
				table[nexti].X = table[i].X;
				table[nexti].L = table[i].L;
				table[nexti].PC = nexti;
				table[nexti].B = table[i].B;
				table[nexti].S = table[i].S;
				table[nexti].T = table[i].T;

				if(table[i].A > 0) cc = 1;
				else if(table[i].A == 0) cc = 0;
				else cc = -1;

				i = nexti;
				break;
			case 48: // JEQ
				if(cc == 0) { // PC
					if(p1[i].object % 16 == 10)
						nexti = i - 1;
					else
						nexti = p1[i].object % (16*16*16) + i + p1[i].format;
				}
				else nexti = i + p1[i].format;
				table[nexti].A = table[i].A;
				table[nexti].X = table[i].X;
				table[nexti].L = table[i].L;
				table[nexti].PC = nexti;
				table[nexti].B = table[i].B;
				table[nexti].S = table[i].S;
				table[nexti].T = table[i].T;

				i = nexti;
				break;
			case 56: // JLT
				if(cc == -1){
					if(p1[i].object % 16 == 3){ // rloop
						nexti = 0x1040;
					}
					else nexti = 0x1062;
				}
				else nexti = i + 3;
				table[nexti].A = table[i].A;
				table[nexti].X = table[i].X;
				table[nexti].L = table[i].L;
				table[nexti].PC = nexti;
				table[nexti].B = table[i].B;
				table[nexti].S = table[i].S;
				table[nexti].T = table[i].T;

				i = nexti;
			    break;
			case 60: // J
			    if(p1[i].n == 1 && p1[i].i == 0) // @
					nexti = progaddr + proglen;
			    else{
				   nexti = 6;
			    }
				table[nexti].A = table[i].A;
				table[nexti].X = table[i].X;
				table[nexti].L = table[i].L;
				table[nexti].PC = nexti;
				table[nexti].B = table[i].B;
				table[nexti].S = table[i].S;
				table[nexti].T = table[i].T;
				
				i = nexti;
			   break;
			case 72: // JSUB
				nexti = p1[i].object % (16*16*16*16*16);
				table[nexti].A = table[i].A;
				table[nexti].X = table[i].X;
				table[nexti].L = i + 4; // next line add
				table[nexti].PC = nexti;
				table[nexti].B = table[i].B;
				table[nexti].S = table[i].S;
				table[nexti].T = table[i].T;

				i = nexti;
			   break;
			case 76: // RSUB
			    nexti = table[i].L;
				table[nexti].A = table[i].A;
				table[nexti].X = table[i].X;
				table[nexti].L = table[i].L;
				table[nexti].PC = nexti;
				table[nexti].B = table[i].B;
				table[nexti].S = table[i].S;
				table[nexti].T = table[i].T;

				i = nexti;
			    break;
			case 84: // STCH
			   nexti = i + 3;
				table[nexti].A = table[i].A;
				table[nexti].X = table[i].X;
				table[nexti].L = table[i].L;
				table[nexti].PC = nexti;
				table[nexti].B = table[i].B;
				table[nexti].S = table[i].S;
				table[nexti].T = table[i].T;

				i = nexti;
			   break;
			case 80: // LDCH
			   nexti = i + 3;
			   table[nexti].A = table[i].A - table[i].A % 256;
			   table[nexti].A += 0x46;
				table[nexti].X = table[i].X;
				table[nexti].L = table[i].L;
				table[nexti].PC = nexti;
				table[nexti].B = table[i].B;
				table[nexti].S = table[i].S;
				table[nexti].T = table[i].T;

				i = nexti;
			   break;
			case 104: // LDB
				nexti = i + p1[i].format;
				table[nexti].A = table[i].A;
				table[nexti].X = table[i].X;
				table[nexti].L = table[i].L;
				table[nexti].PC = nexti;
				table[nexti].S = table[i].S;
				table[nexti].T = table[i].T;
				table[nexti].B = p1[i].object % (16*16*16) + nexti;

				i = nexti;
			   break;
			case 116: // LDT
			    nexti = i + p1[i].format;
			    if(p1[i].format == 4)
				    table[nexti].T = 4096;
			    else table[nexti].T = length;

				table[nexti].A = table[i].A;
				table[nexti].X = table[i].X;
				table[nexti].L = table[i].L;
				table[nexti].PC = nexti;
				table[nexti].B = table[i].B;
				table[nexti].S = table[i].S;

				i = nexti;
			   break;
			case 160: // COMPR
			   nexti = i + 2; cc = 0;
				table[nexti].A = table[i].A;
				table[nexti].X = table[i].X;
				table[nexti].L = table[i].L;
				table[nexti].PC = nexti;
				table[nexti].B = table[i].B;
				table[nexti].S = table[i].S;
				table[nexti].T = table[i].T;

			   i = nexti;
			   break;
			case 180: // CLEAR
			    nexti = i + 2;

				table[nexti].A = table[i].A;
				table[nexti].X = table[i].X;
				table[nexti].L = table[i].L;
				table[nexti].PC = nexti;
				table[nexti].B = table[i].B;
				table[nexti].S = table[i].S;
				table[nexti].T = table[i].T;

				switch((p1[i].object%256 - p1[i].object%16) / 16){
					case 0: // A
						table[nexti].A = 0;
						break;
					case 1: // X
						table[nexti].X = 0;
						break;
					case 4: // S
						table[nexti].S = 0;
						break;
				}

				i = nexti;
			   break;
			case 184: // TIXR
			   nexti = i + 2;
			   table[nexti].X = table[i].X + 1;

				table[nexti].A = table[i].A;
				table[nexti].L = table[i].L;
				table[nexti].PC = nexti;
				table[nexti].B = table[i].B;
				table[nexti].S = table[i].S;
				table[nexti].T = table[i].T;

				if(table[nexti].X > table[nexti].T) cc = 1;
				else if(table[nexti].X == table[nexti].T) cc = 0;
				else cc = -1;

			   i = nexti;
			   break;
			case 216: // RD
			    nexti = i + 3;
				table[nexti].A = table[i].A - table[i].A % 16;
			   
				table[nexti].X = table[i].X;
				table[nexti].L = table[i].L;
				table[nexti].PC = nexti;
				table[nexti].B = table[i].B;
				table[nexti].S = table[i].S;
				table[nexti].T = table[i].T;

				i = nexti;
			    break;
			case 220: // WD
				nexti = i + 3;
				table[nexti].A = table[i].A;
				table[nexti].X = table[i].X;
				table[nexti].L = table[i].L;
				table[nexti].PC = nexti;
				table[nexti].B = table[i].B;
				table[nexti].S = table[i].S;
				table[nexti].T = table[i].T;

				i = nexti;
			   break;
			case 224: // TD
			    nexti = i + 3; cc = -1;

				table[nexti].A = table[i].A;
				table[nexti].X = table[i].X;
				table[nexti].L = table[i].L;
				table[nexti].PC = nexti;
				table[nexti].B = table[i].B;
				table[nexti].S = table[i].S;
				table[nexti].T = table[i].T;

				i = nexti;
			    break;
		}
		//printf("A : %06X   X : %06X\n", table[i].A, table[i].X);
		//printf("L : %06X  PC : %06X\n", table[i].L, table[i].PC);
		//printf("B : %06X   S : %06X\n", table[i].B, table[i].S);
		//printf("T : %06X   cc: %d\n", table[i].T, cc);
		
		for(int k = 0;k < bpidx;k++){
			if(bparr[k][0] == 0 && bparr[k][1] == i){ 
				printf("A : %06X   X : %06X\n", table[i].A, table[i].X);
				printf("L : %06X  PC : %06X\n", table[i].L, table[i].PC);
				printf("B : %06X   S : %06X\n", table[i].B, table[i].S);
				printf("T : %06X\n", table[i].T);
				printf("           Stop at checkpoint[%X]\n", i);

				bparr[k][0] = 1; // change state - used
				return 1;
			}
		}
	}
	if(i == progaddr + proglen){ // run all
		printf("A : %06X   X : %06X\n", table[i].A, table[i].X);
		printf("L : %06X  PC : %06X\n", table[i].L, table[i].PC);
		printf("B : %06X   S : %06X\n", table[i].B, table[i].S);
		printf("T : %06X\n", table[i].T);
		printf("            End Program\n");

		i = progaddr; cc = 0;
		for(int k = 0;k < bpidx;k++)
			bparr[k][0] = 0;

		return 1;
	}
}

20191657.out:	20191657.o 20191657.h
			gcc -Wall -o 20191657.out 20191657.o 20191657.h
			
20191657.o:	20191657.c
			gcc -c 20191657.c

clean:
		rm -f 20191657 20191657.o 20191657.h

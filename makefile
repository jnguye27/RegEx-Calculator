regex: regex.c
	gcc -Wall -std=c99 -pedantic -o regex regex.c

clean: 
	rm regex
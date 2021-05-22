# -c is used to compile source files without linking
# -o is used to generate output file, which is necessary in all cases
# Input file, then -o, THEN output file
# -fPIC generates position independent code for shared files
# -shared will compile as shared library. Do this AFTER fPIC
# -ldl needed for dynamic linking
# -Wl,-rpath= tells executable to load library from what it equals

#Included from Makefile
OPTIMIZE = -O2
CC = gcc
CFLAGS = $(OPTIMIZE) -g3 -Wall -Wextra -march=native -mtune=native -mrdrnd

randmain: randmain.c randcpuid.c
	$(CC) $(CFLAGS) -c randmain.c -o randmain.o
	$(CC) $(CFLAGS) -c randcpuid.c -o randcpuid.o
	$(CC) $(CFLAGS) -ldl -Wl,-rpath=$(PWD) randmain.o randcpuid.o -o randmain
randlibhw.so: randlibhw.c
	$(CC) $(CFLAGS) -fPIC -c randlibhw.c -o randlibhw.o
	$(CC) $(CFLAGS) -shared randlibhw.o -o randlibhw.so
randlibsw.so: randlibsw.c
	$(CC) $(CFLAGS) -fPIC -c randlibsw.c -o randlibsw.o
	$(CC) $(CFLAGS) -shared randlibsw.o -o randlibsw.so

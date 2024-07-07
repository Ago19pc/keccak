clear
gcc -c main.c -o main.o
as keccak.s -o keccak.o
gcc keccak.o main.o -o test -lm
./test

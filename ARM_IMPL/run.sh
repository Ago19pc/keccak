clear
gcc -mcpu=native -fno-stack-protector -O3 main.c -o test -lm
./test

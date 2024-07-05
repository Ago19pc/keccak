clear
gcc -mavx2 -mavx512vl -O2 -pg -no-pie -fno-builtin main.c -o test -lm
./test
clear
gcc -mavx2 -mavx512vl -O3 -march=native -mtune=native -pg -no-pie -fno-builtin main.c -o test -lm
./test
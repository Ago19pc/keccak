clear
gcc -mavx2 -mavx512vl -mtune=native -march=native -O3 -pg -no-pie -fno-builtin main.c -o test -lm
./test
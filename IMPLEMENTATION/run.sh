clear
gcc -mavx2 -mavx512vl -pg -g -no-pie -fno-builtin -fno-stack-protector main.c -o test -lm
./test
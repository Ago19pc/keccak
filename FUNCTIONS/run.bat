clear
gcc -mavx2 -mavx512vl -pg -no-pie -fno-builtin main.c -o test
.\test.exe

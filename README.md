TO COMPILE

gcc -mavx2 -mavx512vl nomefile.c

USE GPROF
gcc -mavx2 -mavx512vl -pg -no-pie -fno-builtin nomefile.c -o nomefile.exe
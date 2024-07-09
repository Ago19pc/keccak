#clears log file
rm -f arm_log.txt
touch arm_log.txt
#compiles the code
echo "Compiling reference code" >> arm_log.txt
gcc -mcpu=native -O3 -fno-builtin REFERENCE/main.c -o REFERENCE/test -lm
echo "Compiling supercop code" >> arm_log.txt
gcc -c -mcpu=native -O3 SUPERCOP_ARM/main.c -o SUPERCOP_ARM/main.o 
as SUPERCOP_ARM/keccak.s -o SUPERCOP_ARM/keccak.o
gcc -mcpu=native -O3 -fno-builtin SUPERCOP_ARM/main.o SUPERCOP_ARM/keccak.o -o SUPERCOP_ARM/test -lm
echo "Compiling ARM implementation code" >> arm_log.txt
gcc -mcpu=native -O3 -pg -no-pie -fno-builtin ARM_IMPL/main.c -o ARM_IMPL/test -lm
#runs the code
echo "Running the code. Please be patient for a while..."
echo "ROUND 1:" >> arm_log.txt
echo "ARM_IMPL:" >> arm_log.txt
./ARM_IMPL/test >> arm_log.txt
echo "SUPERCOP_ARM:" >> arm_log.txt
./SUPERCOP_ARM/test >> arm_log.txt
echo "REFERENCE:" >> arm_log.txt
./REFERENCE/test >> arm_log.txt
#echo "ROUND 2:" >> arm_log.txt
#echo "ARM_IMPL:" >> arm_log.txt
#./ARM_IMPL/test >> arm_log.txt
#echo "SUPERCOP_ARM:" >> arm_log.txt
#./SUPERCOP_ARM/test >> arm_log.txt
#echo "REFERENCE:" >> arm_log.txt
#./REFERENCE/test >> arm_log.txt
#echo "ROUND 3:" >> arm_log.txt
#echo "ARM_IMPL:" >> arm_log.txt
#./ARM_IMPL/test >> arm_log.txt
#echo "SUPERCOP_ARM:" >> arm_log.txt
#./SUPERCOP_ARM/test >> arm_log.txt
#echo "REFERENCE:" >> arm_log.txt
#./REFERENCE/test >> arm_log.txt
#echo "ROUND 4:" >> arm_log.txt
#echo "ARM_IMPL:" >> arm_log.txt
#./ARM_IMPL/test >> arm_log.txt
#echo "SUPERCOP_ARM:" >> arm_log.txt
#./SUPERCOP_ARM/test >> arm_log.txt
#echo "REFERENCE:" >> arm_log.txt
#./REFERENCE/test >> arm_log.txt
#echo "ROUND 5:" >> arm_log.txt
#echo "ARM_IMPL:" >> arm_log.txt
#./ARM_IMPL/test >> arm_log.txt
#echo "SUPERCOP_ARM:" >> arm_log.txt
#./SUPERCOP_ARM/test >> arm_log.txt
#echo "REFERENCE:" >> arm_log.txt
#./REFERENCE/test >> arm_log.txt

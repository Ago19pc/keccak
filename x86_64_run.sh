#clears log file
rm -f x86_64_log.txt
touch x86_64_log.txt
#compiles the code
echo "Compiling reference code" >> x86_64_log.txt
gcc -mavx2 -mavx512vl -mtune=native -march=native -O3 -pg -no-pie -fno-builtin REFERENCE/main.c -o REFERENCE/test -lm
echo "Compiling supercop code" >> x86_64_log.txt
gcc -mavx2 -mavx512vl -mtune=native -march=native -O3 -pg -no-pie -fno-builtin SUPERCOP/main.c -o SUPERCOP/test -lm
echo "Compiling AVX implementation code" >> x86_64_log.txt
gcc -mavx2 -mavx512vl -mtune=native -march=native -O3 -pg -no-pie -fno-builtin AVX_IMPL/main.c -o AVX_IMPL/test -lm
#runs the code
echo "Running the code. Please be patient for a while..."
echo "ROUND 1:" >> x86_64_log.txt
echo "AVX_IMPL:" >> x86_64_log.txt
./AVX_IMPL/test >> x86_64_log.txt
echo "SUPERCOP:" >> x86_64_log.txt
./SUPERCOP/test >> x86_64_log.txt
echo "REFERENCE:" >> x86_64_log.txt
./REFERENCE/test >> x86_64_log.txt
echo "ROUND 2:" >> x86_64_log.txt
echo "AVX_IMPL:" >> x86_64_log.txt
./AVX_IMPL/test >> x86_64_log.txt
echo "SUPERCOP:" >> x86_64_log.txt
./SUPERCOP/test >> x86_64_log.txt
echo "REFERENCE:" >> x86_64_log.txt
./REFERENCE/test >> x86_64_log.txt
echo "ROUND 3:" >> x86_64_log.txt
echo "AVX_IMPL:" >> x86_64_log.txt
./AVX_IMPL/test >> x86_64_log.txt
echo "SUPERCOP:" >> x86_64_log.txt
./SUPERCOP/test >> x86_64_log.txt
echo "REFERENCE:" >> x86_64_log.txt
./REFERENCE/test >> x86_64_log.txt
echo "ROUND 4:" >> x86_64_log.txt
echo "AVX_IMPL:" >> x86_64_log.txt
./AVX_IMPL/test >> x86_64_log.txt
echo "SUPERCOP:" >> x86_64_log.txt
./SUPERCOP/test >> x86_64_log.txt
echo "REFERENCE:" >> x86_64_log.txt
./REFERENCE/test >> x86_64_log.txt
echo "ROUND 5:" >> x86_64_log.txt
echo "AVX_IMPL:" >> x86_64_log.txt
./AVX_IMPL/test >> x86_64_log.txt
echo "SUPERCOP:" >> x86_64_log.txt
./SUPERCOP/test >> x86_64_log.txt
echo "REFERENCE:" >> x86_64_log.txt
./REFERENCE/test >> x86_64_log.txt

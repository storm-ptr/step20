${1:-g++} -I ../../.. -std=c++20 -Wall -O2 -o result main.cpp
./result ./file1.txt ./file2.txt
rm ./result

cl /I  ../../.. /std:c++20 /EHsc /nologo /O2 main.cpp
main.exe ./file1.txt ./file2.txt
del main.obj main.exe

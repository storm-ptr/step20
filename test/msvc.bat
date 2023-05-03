@ECHO OFF
SET compiler=%1
IF "%1"=="" (
    SET compiler=cl
)
@ECHO ON
%compiler% /I  ../.. /std:c++20 /EHsc /nologo /O2 main.cpp
main.exe
del main.obj main.exe

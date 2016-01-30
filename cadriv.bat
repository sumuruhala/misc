@echo off

pushd %learn%\C++

set compiler= D:\LLVM\bin\clang++.exe
set driver= %learn%\python\cadriv.py
set inputfile= .\calc_tmp_expr_in.txt
set cpptmp= .\calc_tmp.cpp
set tempcpp= .\__cadriv_temporary.cpp
set outputfile= .\tmp_dump.txt

copy %cpptmp% %tempcpp%

call python %driver% "run" %inputfile% %tempcpp%

call %compiler% -std=c++14 %tempcpp% 2> %outputfile%

call python %driver% "res" %outputfile% >> %inputfile%

del %tempcpp%

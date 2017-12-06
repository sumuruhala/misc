# Miscellaneous Repository

### Program 1 : A simple compile-time calculator leveraging C++ Template Meta-Programming mechanism.
| Files                 | Description           |
| --------------------- |-----------------------|
| calc_tmp.cpp          | parses and calculates simple arithmetic expression, e.g. "(1+2)*(3-4)/5%6+(1!-((2^3)^4))/2!!". All of the calculation is done in C++ COMPILE-time                 |
| cadriv.py             | stands for "CAlculator DRIVer", which is responsible for driving the C++ file/compiler to do the job as well as for handling the results. Input is accepted in a file named "calc_tmp_expr_in.txt", and the result is output in the same file                 |
| cadriv.bat            | Windows batch file to run the program                 |

### Program 2 : A simple console calculator implemented in C++11 and Erlang, respectively.
| Files                 |
| --------------------- |
| calc.cpp              |
| calc2.erl             |

### Program 3 : Compile-time (template metaprogramming) heap sort. Sort result is dumped via error messages.
| Files                 |
|-----------------------|
| tmp_heap.cpp          |

# The Repository of Miscellaneous Code

### Program 1 : A simple compile-time calculator leveraging C++ Template Meta-Programming.
| Files                 | Description           |
| --------------------- |-----------------------|
| calc_tmp.cpp          | Parses and calculates simple arithmetic expression, e.g. "(1+2)*(3-4)/5%6+(1!-((2^3)^4))/2!!". All of the calculation is done during C++ COMPILE-time                 |
| cadriv.py             | Stands for "CAlculator DRIVer", which is responsible for driving the C++ source & compiler to "execute" the program, as well as for handling the result. Input is accepted in a file named "calc_tmp_expr_in.txt", and the result is output to the same file                 |
| cadriv.bat            | A sample script (Windows batch file) to run the whole program                 |

### Program 2 : A simple console calculator implemented in C++11 and Erlang, respectively.
| Files                 |
| --------------------- |
| calc.cpp              |
| calc2.erl             |

### Program 3 : A compile-time/template meta-programming Heap Sort implementation. Compile to "run" the program, and the sorting result is dumped via compiler's error messages.
| Files                 |
|-----------------------|
| tmp_heap.cpp          |

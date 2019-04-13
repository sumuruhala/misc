# The Repository of Miscellaneous Code

### Program 1 : A simple compile-time calculator leveraging C++ Template Meta-Programming
| Files                 | Description           |
| --------------------- |-----------------------|
| calc_tmp.cpp          | Parses and calculates simple arithmetic expression, e.g. "(1+2)*(3-4)/5%6+(1!-((2^3)^4))/2!!". All of the calculation is done during C++ COMPILE-time                 |
| cadriv.py             | Stands for "CAlculator DRIVer", which is responsible for driving the C++ source & compiler to "execute" the program, as well as for handling the result. Input is accepted in a file named "calc_tmp_expr_in.txt", and the result is output to the same file                 |
| cadriv.bat            | A sample script (Windows batch file) to run the whole program                 |

### Program 2 : A compile-time/template meta-programming Heap Sort implementation
| Files                 | Description           |
|-----------------------|-----------------------|
| tmp_heap.cpp          | Compile to "run" the program, and the sorting result is dumped via compiler's error messages. |

### Program 3 : A simple recursive-descent-parsing console calculator
| Files                 | Description           |
| --------------------- |-----------------------|
| calc.cpp              | C++11 implementation  |
| calc2.erl             | Erlang implementation |


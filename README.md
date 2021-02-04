# Sudoku-puzzle-verifier
Simple program written in C which verifies whether or not a sudoku puzzle solution is valid or not.
This is accomplished by making use of multi-threading.

Usage: SudokuPuzzle.txt is a sample input file. Users are reccomended to alter this file before execution or use it as reference when creating your own input file.

This program is natively executable by most Unix/Linux/OS X users

If creating your own executable:
```
   gcc sValidator.c -o (name of executable file) -lpthread
   ./(name of executable file)
```
Else:
```
  ./sValidator
```

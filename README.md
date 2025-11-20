##
## README.md
## Name: Braydon Johnston REDid: 131049942 Class Acc: cssc2115
## Name: Reinaldo Roldan REDid: 825760540 Class Acc: cssc2131
## CS530-03 Fall 2025
##

## files (pre-running)
# include folder
-assembler.hpp
-listing.hpp
-optab.hpp
-pass1.hpp
-pass2.hpp
-symtab.hpp
-utils.hpp
# src folder
-assembler.cpp
-listing.cpp
-optab.cpp
-pass1.cpp
-pass2.cpp
-symtab.cpp
-utils.cpp
# tests folder
-fig2-9.sic
-fig2-15.sic
-P2sample.sic
-test1.sic
-test2.sic
# other
-Makefile
-README.md
# (post-running)
-obj
-asx
# tests(output files)
-fig2-9.l
-fig2-15.l
-P2sample.l
-test1.l
-test2.l
-fig2-9.st
-fig2-15.st
-P2sample.st
-test1.st
-test2.st

## Compile instructions
# Makefile instructions:
-cd a2 and type:
	make

# assembly instructions:(all test files)
-cd a2 and type:
	./asx tests/fig2-9.sic tests/fig2-15.sic tests/P2sample.sic tests/test1.sic tests/test2.sic

# Significant Design Decision: 
-Our output at first was not organized, even though we got the right output we decided to clean it up by organizing the data into columns 

# Known Deficiencies/Bugs:
-There aren't any bugs that are known to us but some deficiencies include when testing code with opcode that our code isn't designed to process, we'll usually catch an error, "Error: invalid opcode ___", the code still gives us correct organized output

# Lessons Learned:
-Managing the Symbol Table is more complicated than it seems
-Duplicate labels are not allowed in SIC/XE code, and having a check for duplicate labels is necessary so that the code can run smoothly
-Better understanding of how assemblers work
-Learned how to handle all addressing formats
-Even small formatting differences can make the output unaligned 
-The handling of literals is important because if they're not handled in literal pools, it can cause the object codes to break or could lead to the program failing.
-Evaluating symbols (absolute and relative values) first is helpful in detecting invalid expressions.
-Testing multiple files is important because different code will make you run into different problems, and debugging helps us write code that can handle more cases
-Had to be very careful when handling addressing modes because the wrong logic would produce incorrect output
-These classes can get pretty long and confusing if comments aren't used correctly
-Small stuff like "*" from P2sample was throwing errors because it was processing it as a label, which it didn't like, giving the error "Invalid opcode," so we handled it in Pass1::processInstruction()
-Debugging is our best friend :D

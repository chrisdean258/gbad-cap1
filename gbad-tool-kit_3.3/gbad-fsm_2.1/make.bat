@echo off
del *.o

g++ -g -Wall -O -c main.cpp
g++ -g -Wall -O -c database.cpp
g++ -g -Wall -O -c path.cpp
g++ -g -Wall -O -c legoccurrence.cpp
g++ -g -Wall -O -c patterntree.cpp
g++ -g -Wall -O -c patterngraph.cpp
g++ -g -Wall -O -c closeleg.cpp
g++ -g -Wall -O -c graphstate.cpp
g++ -g -Wall -O -c gbad.cpp
g++ -g -Wall -O -c actions.cpp
gcc -g -Wall -O -c lex.yy.c
g++ -g -Wall -O -c y.tab.c
g++ -g -Wall -O -o gbad-fsm main.o database.o path.o legoccurrence.o patterntree.o patterngraph.o closeleg.o graphstate.o gbad.o actions.o lex.yy.o y.tab.o
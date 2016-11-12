# Makefile
#
# GBAD-FSM 0.1
#
GCC=g++ -g -Wall -O

ALLFILES=main.cpp database.cpp database.h misc.h patterntree.cpp patterntree.h closeleg.cpp closeleg.h patterngraph.cpp patterngraph.h graphstate.cpp graphstate.h gbad.cpp gbad.h

OBJS=main.o database.o path.o legoccurrence.o patterntree.o patterngraph.o closeleg.o graphstate.o gbad.o actions.o lex.yy.o y.tab.o

gbad-fsm: $(OBJS)
	$(GCC) -o gbad-fsm $(OBJS) 
clean:
	rm $(OBJS); rm gbad-fsm
database.o: database.cpp database.h Label.h LabelList.h
	$(GCC) -c database.cpp
database.h: legoccurrence.h misc.h
patterntree.o: patterntree.cpp patterntree.h patterngraph.h graphstate.h gbad.h
	$(GCC) -c patterntree.cpp
patterntree.h: misc.h database.h legoccurrence.h path.h closeleg.h
patterngraph.o: patterngraph.cpp patterngraph.h graphstate.h gbad.h
	$(GCC) -c patterngraph.cpp
patterngraph.h: closeleg.h
main.o: misc.h database.h main.cpp path.h graphstate.h gbad.h
	$(GCC) -c main.cpp
legoccurrence.o: legoccurrence.h legoccurrence.cpp closeleg.h database.h graphstate.h gbad.h
	$(GCC) -c legoccurrence.cpp
legoccurrence.h: misc.h
closeleg.o: closeleg.cpp closeleg.h misc.h graphstate.h gbad.h
	$(GCC) -c closeleg.cpp
closeleg.h: misc.h legoccurrence.h
path.o: path.cpp path.h patterntree.h patterngraph.h graphstate.h gbad.h
	$(GCC) -c path.cpp
path.h: misc.h database.h legoccurrence.h closeleg.h
graphstate.o: graphstate.cpp graphstate.h database.h misc.h gbad.h
	$(GCC) -c graphstate.cpp
graphstate.h: misc.h 
gbad.o: gbad.cpp gbad.h graphstate.h database.h misc.h
	$(GCC) -c gbad.cpp
gbad.h: misc.h

lex.yy.o : lex.yy.h lex.yy.c
	gcc -g -Wall -O -c lex.yy.c

y.tab.o : y.tab.h y.tab.c
	$(GCC) -c y.tab.c

actions.o : actions.h actions.cpp lex.yy.h y.tab.h LabelList.h Label.h
	$(GCC) -c actions.cpp

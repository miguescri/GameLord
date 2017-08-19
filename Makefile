CPP			:= g++
CPPFLAG	:= -std=c++11 -Wall
BIN			:= bin
MAIN		:= source/main
CORE		:= source/core


.PHONY:all
all: ${BIN}/main.o
	${CPP} -o ${BIN}/test ${BIN}/main.o

test_board: ${BIN}/test_board.o
	${CPP} -o ${BIN}/test_board ${BIN}/main.o

${BIN}/test_board.o: ${MAIN}/test_board.cpp ${CORE}/board.h ${CORE}/boardexception.h
	${CPP} -o bin/test_board.o -c ${MAIN}/test_board.cpp ${CPPFLAG}
.PHONY:docs
docs:
	doxygen

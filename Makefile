CPP			:= g++
CPPFLAG	:= -std=c++11 -Wall
BIN			:= bin
MAIN		:= source/main
CORE		:= source/core


.PHONY:all
all: ${BIN}/main.o
	${CPP} -o ${BIN}/test ${BIN}/main.o

${BIN}/main.o: ${MAIN}/test_main.cpp ${CORE}/board.h
	${CPP} -o bin/main.o -c ${MAIN}/test_main.cpp ${CPPFLAG}

.PHONY:docs
docs:
	doxygen

CPP			:= g++
CPPFLAG	:= -std=c++11 -Wall
BIN			:= bin
MAIN		:= source/main
CORE		:= source/core


.PHONY:all
all: test_board test_defboard

test_board: ${BIN}/test_board.o
	${CPP} -o ${BIN}/test_board ${BIN}/main.o

test_defboard: ${BIN}/test_defboard.o
	${CPP} -o ${BIN}/test_defboard ${BIN}/main.o


${BIN}/test_board.o: ${MAIN}/test_board.cpp ${CORE}/board.h ${CORE}/boardexception.h
	${CPP} -o bin/test_board.o -c ${MAIN}/test_board.cpp ${CPPFLAG}

${BIN}/test_defboard.o: ${MAIN}/test_defboard.cpp DefBoard
	${CPP} -o bin/test_defboard.o -c ${MAIN}/test_defboard.cpp ${CPPFLAG}

DefBoard: ${CORE}/defboard.h ${CORE}/board.h ${CORE}/boardexception.h

.PHONY:docs
docs:
	doxygen

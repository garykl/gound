CPP = g++ -D__LINUX_ALSA__ -Wall -pedantic --std=c++11

INC = -I/usr/include/stk -I./src
LNK = -lstk

SRCS = $(wildcard synths/*.c)
PROGS := $(patsubst synths/%.c,%,$(SRCS))

metalBass: synths/metalBass.cc ${OBJ}
	${CPP} ${INC} synths/metalBass.cc -o bin/$@ ${LNK}

main: synths/main.cc ${OBJ}
	${CPP} ${INC} synths/main.cc -o bin/$@ ${LNK}


all: metalBass main

CPP = g++ -D__LINUX_ALSA__ -Wall -pedantic --std=c++11

INC = -I/usr/include/stk -I./src
LNK = -lstk



main: synths/main.cc ${OBJ}
	${CPP} ${INC} synths/main.cc -o bin/$@ ${LNK}

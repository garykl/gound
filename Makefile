CPP = g++ -D__LINUX_ALSA__ -Wall -pedantic --std=c++11

INC = -I/usr/include/stk -I./src
LNK = -lstk

metalBass: synths/metalBass.cc ${OBJ}
	${CPP} ${INC} synths/metalBass.cc -o bin/$@ ${LNK}

twoVoices: synths/twoVoices.cc ${OBJ}
	${CPP} ${INC} synths/twoVoices.cc -o bin/$@ ${LNK}

main: synths/main.cc ${OBJ}
	${CPP} ${INC} synths/main.cc -o bin/$@ ${LNK}


all: twoVoices metalBass main

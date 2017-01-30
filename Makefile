CC= g++
CPPFLAGS= -D__LINUX_ALSA__ -Wall -pedantic --std=c++11 -I/usr/include/stk -I./src -lstk
SRCFILES= $(wildcard synths/*.cc)
OBJFILES= $(patsubst %.cc, %.o, $(SRCFILES))
PROGFILES= $(patsubst %.cc, %, $(SRCFILES))

all: $(PROGFILES)

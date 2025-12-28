EXE                = eris
VERSION            = 0.2
EXE_NAME           = $(EXE)-$(VERSION)
CC                 = g++
SRC                = *.cpp

ifeq ($(OS), Windows_NT)
	EXT = .exe
else
	EXT = 
endif

WFLAGS = -Wall -Wno-unused-function -Wno-switch -Wno-attributes -Wno-missing-field-initializers
FLAGS = $(WFLAGS) -s -flto -O3 -funroll-loops
LIBS =

ifeq ($(EXT), .exe)
	FLAGS += -static -static-libgcc -static-libstdc++
endif

eris:
	$(CC)   $(SRC) -march=native -o $(EXE_NAME)$(EXT)

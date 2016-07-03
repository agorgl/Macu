PRJTYPE = Executable
LIBS = macu
ifeq ($(OS), Windows_NT)
	LIBS += user32 ws2_32
endif
ADDINCS = ../include
ADDLIBDIR = ../lib

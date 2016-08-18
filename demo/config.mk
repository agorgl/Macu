PRJTYPE = Executable
LIBS = macu
ifeq ($(OS), Windows_NT)
	LIBS += user32 ws2_32
else
	LIBS += pthread
endif
ADDINCS = ../include
ADDLIBDIR = ../lib

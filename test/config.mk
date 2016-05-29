PRJTYPE = Executable
LIBS = macu
ifeq ($(OS), Windows_NT)
	LIBS += user32
endif
ADDINCS = ../include
ADDLIBDIR = ../lib

PRJTYPE = Executable
LIBS = macu
ifeq ($(OS), Windows_NT)
	LIBS += user32
else
	LIBS += pthread
endif
ADDINCS = ../include
ADDLIBDIR = ../lib

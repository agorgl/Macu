PRJTYPE = Executable
LIBS = macu
ifeq ($(TARGET_OS), Windows_NT)
	LIBS += user32
else
	LIBS += pthread
endif
MOREDEPS = ..

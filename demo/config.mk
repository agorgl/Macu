PRJTYPE = Executable
LIBS = macu
ifeq ($(TARGET_OS), Windows_NT)
	LIBS += user32 ws2_32
else
	LIBS += pthread
endif
MOREDEPS = ..
